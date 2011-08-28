#include <cstring>
#include <cassert>
#include "eapol.h"
#include "utils.h"


Tins::EAPOL::EAPOL(uint8_t packet_type, uint8_t type) : PDU(0xff) {
    std::memset(&_header, 0, sizeof(_header));
    _header.version = 1;
    _header.packet_type = packet_type;
    _header.type = type;
}

void Tins::EAPOL::version(uint8_t new_version) {
    _header.version = new_version;
}
        
void Tins::EAPOL::packet_type(uint8_t new_ptype) {
    _header.packet_type = new_ptype;
}

void Tins::EAPOL::length(uint8_t new_length) {
    _header.length = Utils::net_to_host_s(new_length);
}

void Tins::EAPOL::type(uint8_t new_type) {
    _header.type = new_type;
}

void Tins::EAPOL::write_serialization(uint8_t *buffer, uint32_t total_sz, const PDU *) {
    uint32_t sz = header_size();
    assert(total_sz >= sz);
    if(!_header.length)
        length(sz - sizeof(_header.version) - sizeof(_header.length) - sizeof(_header.type));
    std::memcpy(buffer, &_header, sizeof(_header));
    write_body(buffer + sizeof(_header), total_sz - sizeof(_header));
}

/* RC4EAPOL */

Tins::RC4EAPOL::RC4EAPOL() : EAPOL(0x03, 0x01), _key(0), _key_size(0) {
    std::memset(&_header, 0, sizeof(_header));
}

Tins::RC4EAPOL::~RC4EAPOL() {
    delete[] _key;
}

void Tins::RC4EAPOL::key_length(uint16_t new_key_length) {
    _header.key_length = new_key_length;
}
        
void Tins::RC4EAPOL::replay_counter(uint16_t new_replay_counter) {
    _header.replay_counter = new_replay_counter;
}

void Tins::RC4EAPOL::key_iv(const uint8_t *new_key_iv) {
    std::memcpy(_header.key_iv, new_key_iv, sizeof(_header.key_iv));
}

void Tins::RC4EAPOL::key_flag(bool new_key_flag) {
    _header.key_flag = new_key_flag;
}

void Tins::RC4EAPOL::key_index(uint8_t new_key_index) {
    _header.key_index = new_key_index;
}

void Tins::RC4EAPOL::key_sign(const uint8_t *new_key_sign) {
    std::memcpy(_header.key_sign, new_key_sign, sizeof(_header.key_sign));
}

void Tins::RC4EAPOL::key(const uint8_t *new_key, uint32_t sz) {
    delete[] _key;
    _key = new uint8_t[sz];
    _key_size = sz;
    std::memcpy(_key, new_key, sz);
}

uint32_t Tins::RC4EAPOL::header_size() const {
    return sizeof(eapolhdr) + sizeof(_header) + _key_size;
}

void Tins::RC4EAPOL::write_body(uint8_t *buffer, uint32_t total_sz) {
    uint32_t sz = sizeof(_header) + _key_size;
    assert(total_sz >= sz);
    if(_key)
        _header.key_length = Utils::net_to_host_s(_key_size);
    std::memcpy(buffer, &_header, sizeof(_header));
    buffer += sizeof(_header);
    if(_key)
        std::memcpy(buffer, _key, _key_size);
}
