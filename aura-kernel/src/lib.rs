use std::ffi::CString;
use std::os::raw::c_char;

#[unsafe(no_mangle)]
pub extern "C" fn aura_kernel_init() -> i32 {
    println!("[Rust Kernel] FFI: Initializing Aura Privacy Shield...");
    1 // Success
}

#[unsafe(no_mangle)]
pub extern "C" fn aura_kernel_get_version() -> *mut c_char {
    let version = CString::new("1.0.0-ffi.alpha").unwrap();
    version.into_raw()
}

#[unsafe(no_mangle)]
pub extern "C" fn aura_kernel_free_string(s: *mut c_char) {
    unsafe {
        if s.is_null() {
            return;
        }
        let _ = CString::from_raw(s);
    }
}

#[unsafe(no_mangle)]
pub extern "C" fn aura_kernel_calculate_fluid_intensity(time: f32) -> f32 {
    // Logika Kernel: Membuat pergerakan lebih "liat" dengan mencampur dua gelombang sinus
    // Menghasilkan efek perlambatan dan percepatan yang tidak konstan
    let ripple = (time * 0.4).sin() * 0.15;
    let wave = (time * 1.2 + ripple).cos() * 0.05;
    time + ripple + wave
}
