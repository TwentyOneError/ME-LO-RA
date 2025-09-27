#include "sha256.h"
#include <string.h>

//Константы алгоритма SHA-256 (хранятся в PROGMEM для экономии RAM)
const uint32_t SHA256::K[64] PROGMEM = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

//Начальные значения хэша
const uint32_t SHA256::H0[8] PROGMEM = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

//Вспомогательные функции
SHA256::WORD SHA256::rotr(WORD x, int n) {
    return (x >> n) | (x << (32 - n));
}

SHA256::WORD SHA256::ch(WORD x, WORD y, WORD z) {
    return (x & y) ^ (~x & z);
}

SHA256::WORD SHA256::maj(WORD x, WORD y, WORD z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

SHA256::WORD SHA256::sigma0(WORD x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

SHA256::WORD SHA256::sigma1(WORD x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

SHA256::WORD SHA256::theta0(WORD x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

SHA256::WORD SHA256::theta1(WORD x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

//Основная функция преобразования блока
void SHA256::transform(WORD state[8], const BYTE block[64]) {
    WORD a, b, c, d, e, f, g, h;
    WORD w[64];
    
    // Разбиваем блок на 16 слов (big-endian)
    for (int i = 0; i < 16; i++) {
        w[i] = ((WORD)block[i*4] << 24) |
               ((WORD)block[i*4+1] << 16) |
               ((WORD)block[i*4+2] << 8) |
               (block[i*4+3]);
    }
    
    // Расширяем до 64 слов
    for (int i = 16; i < 64; i++) {
        w[i] = theta1(w[i-2]) + w[i-7] + theta0(w[i-15]) + w[i-16];
    }
    
    // Инициализация рабочих переменных
    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];
    f = state[5];
    g = state[6];
    h = state[7];
    
    // Цикл сжатия
    for (int i = 0; i < 64; i++) {
        WORD k_val = pgm_read_dword_near(K + i);
        WORD T1 = h + sigma1(e) + ch(e, f, g) + k_val + w[i];
        WORD T2 = sigma0(a) + maj(a, b, c);
        
        h = g;
        g = f;
        f = e;
        e = d + T1;
        d = c;
        c = b;
        b = a;
        a = T1 + T2;
    }
    
    // Обновление состояния
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
    state[5] += f;
    state[6] += g;
    state[7] += h;
}

// Основная функция хэширования
String SHA256::hash(const uint8_t* data, size_t len) {
    WORD state[8];
    // Инициализация состояния
    for (int i = 0; i < 8; i++) {
        state[i] = pgm_read_dword_near(H0 + i);
    }

    // Обработка полных блоков
    size_t block_count = len / 64;
    for (size_t i = 0; i < block_count; i++) {
        transform(state, data + i * 64);
    }

    // Подготовка последнего блока
    BYTE block[64] = {0};
    size_t remaining = len % 64;
    memcpy(block, data + block_count * 64, remaining);
    
    // Добавляем бит "1"
    block[remaining] = 0x80;
    
    // Длина сообщения в битах
    UINT64 bit_length = static_cast<UINT64>(len) * 8;
    
    // Если места недостаточно для длины сообщения
    if (remaining >= 56) {
        transform(state, block);
        memset(block, 0, 64);
    }
    
    // Добавляем длину сообщения (big-endian)
    for (int i = 0; i < 8; i++) {
        block[56 + i] = (bit_length >> (56 - i * 8)) & 0xFF;
    }
    transform(state, block);

    // Формируем результат в виде hex-строки
    String result = "";
    for (int i = 0; i < 8; i++) {
        char buf[9];
        snprintf(buf, sizeof(buf), "%08lx", state[i]);
        result += buf;
    }
    
    return result;
}

//Для String
String SHA256::hash(const String& input) {
    return hash(reinterpret_cast<const uint8_t*>(input.c_str()), input.length());
}
