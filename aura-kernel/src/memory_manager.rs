/// Manajemen Memori Kernel Aura OS
/// Modul ini bertanggung jawab atas alokasi memori yang terisolasi untuk proses
/// guna memastikan prinsip privasi di dalam perangkat (on-device).

#[derive(Debug, PartialEq)]
pub enum MemoryError {
    OutOfMemory,
    InvalidAlignment,
    SecurityViolation,
}

/// Representasi wilayah memori yang dialokasikan untuk sebuah proses.
#[derive(Debug, Clone, Copy)]
pub struct ProcessMemoryMap {
    pub base_address: usize,
    pub size_limit: usize,
}

pub struct MemoryManager {
    total_capacity: usize,
    allocated_bytes: usize,
}

impl MemoryManager {
    /// Membuat manajer memori baru dengan kapasitas tertentu.
    pub fn new(capacity: usize) -> Self {
        Self {
            total_capacity: capacity,
            allocated_bytes: 0,
        }
    }

    /// Mengalokasikan wilayah memori baru untuk proses.
    /// Fungsi ini mengikuti aturan ketat Rust tanpa menggunakan blok `unsafe`.
    pub fn allocate_region(&mut self, size: usize) -> Result<ProcessMemoryMap, MemoryError> {
        // Simulasi pemeriksaan alokasi memori yang aman
        if size == 0 {
            return Err(MemoryError::InvalidAlignment);
        }

        if self.allocated_bytes + size > self.total_capacity {
            return Err(MemoryError::OutOfMemory);
        }

        // Dalam implementasi kernel nyata, ini akan berinteraksi dengan sistem MMU.
        // Di sini kita mensimulasikan pemetaan alamat secara logis.
        let new_region = ProcessMemoryMap {
            base_address: 0x4000_0000 + self.allocated_bytes, // Alamat virtual simulasi
            size_limit: size,
        };

        self.allocated_bytes += size;

        println!(
            "[Kernel Memory] Alokasi berhasil: Base 0x{:x}, Size {} bytes",
            new_region.base_address, new_region.size_limit
        );

        Ok(new_region)
    }

    /// Mengambil status penggunaan memori saat ini.
    pub fn get_usage(&self) -> (usize, usize) {
        (self.allocated_bytes, self.total_capacity)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_successful_allocation() {
        let mut manager = MemoryManager::new(1024);
        let region = manager.allocate_region(512).unwrap();
        assert_eq!(region.size_limit, 512);
        assert_eq!(manager.get_usage().0, 512);
    }

    #[test]
    fn test_out_of_memory() {
        let mut manager = MemoryManager::new(100);
        let result = manager.allocate_region(150);
        assert_eq!(result.err(), Some(MemoryError::OutOfMemory));
    }

    #[test]
    fn test_invalid_alignment() {
        let mut manager = MemoryManager::new(100);
        let result = manager.allocate_region(0);
        assert_eq!(result.err(), Some(MemoryError::InvalidAlignment));
    }
}
