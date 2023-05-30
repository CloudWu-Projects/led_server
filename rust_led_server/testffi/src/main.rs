use libc::c_int;
use std::ffi::OsStr;

#[cfg(target_os="linux")]
use libloading::os::unix::Symbol as RawSymbol;
#[cfg(target_os="windows")]
use libloading::os::windows::Symbol as RawSymbol;


use libloading::{Library, Symbol};

#[repr(C)]
struct Object {
    _private: [u8; 0],
}


type    LedServerCallback=unsafe extern "C"  fn(Msg:c_int, wParam:c_int, lParam:*const u8)->c_int;
type    LV_InitServerFn =unsafe extern "C" fn(port: c_int) -> c_int;



struct LedWrapper{
    lib: Library,
    pub init_server_fn: RawSymbol<LV_InitServerFn> ,
}

impl LedWrapper{
    fn new() -> Self {

    
        
    
    
        let lib_path = match (std::env::consts::OS,std::env::consts::ARCH) {        
            ("windows", "x86_64") =>"lv_led_MBCS_64.dll",
            ("windows", "x86") =>"lv_led_MBCS_32.dll",
            ("linux","x86_64") => "/home/admin/libledplayer7.so",
            _ => panic!("Unsupported operating system"),
        };
        println!("Loading {}", lib_path);

        let lib=unsafe{ Library::new(lib_path).unwrap()};

        let funcName_LV_LedInitServer=match std::env::consts::OS {
            "windows" => "LV_LedInitServer\0",
            _ => "_Z13LV_InitServeri\0",
        };
        
        println!("funcName_LV_LedInitServer funcName {}", funcName_LV_LedInitServer);
        let init_server_fnx: Symbol<LV_InitServerFn> = unsafe { lib.get(funcName_LV_LedInitServer.as_bytes()).unwrap() };
        let init_server_fnx =  unsafe { init_server_fnx.into_raw()};

        LedWrapper{
            lib: lib,
            init_server_fn: init_server_fnx,
        }
    }
}
fn main() {
    // Load the shared library
    
    let wrapper = LedWrapper::new();
    
    // Call the function
    let port = 1234;
    let result = unsafe { (wrapper.init_server_fn)(port) };
    if result != 0 {
        panic!("Failed to initialize server: {}", result);
    }
    println!("Server initialized on port {}", port);
    loop {
        std::thread::sleep(std::time::Duration::from_secs(100));
    }
}
