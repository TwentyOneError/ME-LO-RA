from random import randint
from math import sqrt
import numpy as np
def primes_numpy(n):
    if n < 2:
        return np.array([], dtype=int)
    sieve = np.ones(n + 1, dtype=bool)
    sieve[0] = sieve[1] = False
    sieve[4::2] = False  # Убираем четные, кроме 2
    for i in range(3, int(n**0.5) + 1, 2):
        if sieve[i]:
            sieve[i*i::i] = False  # Векторизованное удаление кратных
    return np.nonzero(sieve)[0]
ma = primes_numpy(2**31)
print(len(ma))
p = ma[randint(300000,len(ma)-1)]#выбираем случайное простое число p;
q = ma[randint(300000,len(ma)-1)]#выбираем случайное простое число q;
while p == q:
    p = ma[randint(300000, len(ma) - 1)]  # выбираем случайное простое число p
n = p*q
phi = (p-1)*(q-1)# вычисляем φ(n)
i = len(ma)-1
flg = 3
while ma[i]>=phi:#находим примерную границу простых чисел меньше φ(n)
    i = i//2
    if ma[i]<phi and flg>0:
        i = i+i//2
        flg-=1
e = ma[randint(500, i - 1)]#выбираем случайное простое число меньшее φ(n)
a = e
while a!=1:
    a = e
    b = phi
    while a != 0 and b != 0:
        if a > b:
            a = a % b
        else:
            b = b % a
    if a!=1:
        e = ma[randint(500, i - 1)]#проверяем на делители
def extended_gcd(a, b):
    if b == 0:
        return a, 1, 0
    gcd, x1, y1 = extended_gcd(b, a % b)
    x = y1
    y = x1 - (a // b) * y1
    return gcd, x, y
gcd, d, y = extended_gcd(e, phi)
if gcd != 1:
    print("Ошибка: e и φ(n) не взаимно просты")
else:
    d = d % phi  # Приводим к положительному виду
print("e",e,sep="")
print("n",n,sep="")
print("d",d,sep="")
input()