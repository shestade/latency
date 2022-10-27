#pragma once

inline unsigned long long rdtsc() {
	unsigned int lo, hi;
	asm volatile ( "rdtsc\n" : "=a" (lo), "=d" (hi) );
	return ((unsigned long long)hi << 32) | lo;
}