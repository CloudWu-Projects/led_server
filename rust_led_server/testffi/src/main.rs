use libloading::{Library, Symbol};

type LV_InitServerFn = unsafe extern "C" fn(port: i32) -> i32;

fn main() {
    // Load the shared library
    let lib_path = match (std::env::consts::OS,std::env::consts::ARCH) {        
        ("windows", "x86_64") =>"lv_led_64.dll",
        ("windows", "x86") =>"lv_led_32.dll",
        ("linux","x86_64") => "libledplayer7.so",
        _ => panic!("Unsupported operating system"),
    };

    println!("Loading {}", lib_path);
    
    let lib=unsafe{ Library::new(lib_path).unwrap()};

    // Get the function pointer
    let init_server_fn: Symbol<LV_InitServerFn> = unsafe { lib.get(b"LV_LedInitServer\0").unwrap() };

    // Call the function
    let port = 1234;
    let result = unsafe { init_server_fn(port) };
    if result != 0 {
        panic!("Failed to initialize server: {}", result);
    }
    println!("Server initialized on port {}", port);
    loop {
        std::thread::sleep(std::time::Duration::from_secs(100));
    }
}
