#ifndef AURA_KERNEL_H
#define AURA_KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Initialize Aura Privacy Shield from Rust
int32_t aura_kernel_init();

// Get Kernel Version string (caller must free using aura_kernel_free_string)
char *aura_kernel_get_version();

// Free string allocated by Rust
void aura_kernel_free_string(char *s);

// Calculate fluid intensity using Kernel logic
float aura_kernel_calculate_fluid_intensity(float time);

#ifdef __cplusplus
}
#endif

#endif // AURA_KERNEL_H
