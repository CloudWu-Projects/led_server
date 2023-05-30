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
type LPAREARECT=*const u8;
type HPROGRAM=*const u8;

type    LedServerCallback=unsafe extern "C"  fn(Msg:c_int, wParam:c_int, lParam:*const u8)->c_int;
type    LedServerRegisterCallback=unsafe extern "C"  fn(call_back:* mut LedServerCallback)->c_int;
type    LV_InitServerFn =unsafe extern "C" fn(port: c_int) -> c_int;
type   LV_CreateProgramEx=unsafe extern "C" fn(LedWidth:c_int,LedHeight:c_int,ColorType:c_int,GrayLevel:c_int,SaveType:c_int)->HPROGRAM;
type  LV_AddProgram=unsafe extern "C" fn(hProgram:HPROGRAM,ProgramNo:c_int, ProgramTime:c_int, LoopCount:c_int)->c_int;
type LV_AddImageTextArea=unsafe extern "C" fn(hProgram:HPROGRAM,ProgramNo:c_int,AreaNo:c_int,pAreaRect:LPAREARECT,IsBackgroundArea:c_int)->c_int;
type LV_DeleteProgram=unsafe extern "C" fn(hProgram:HPROGRAM );

struct LedWrapper{
    lib: Library,
    init_server_fn: RawSymbol<LV_InitServerFn> ,
    register_callback: RawSymbol<LedServerRegisterCallback>, 
    create_program_ex: RawSymbol<LV_CreateProgramEx>,
    add_program: RawSymbol<LV_AddProgram>,
    add_image_text_area: RawSymbol<LV_AddImageTextArea>,
    delete_program: RawSymbol<LV_DeleteProgram>,

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

        let register_callback_fun: Symbol<LedServerRegisterCallback> = unsafe { lib.get(b"_Z25LV_RegisterServerCallbackPFiiiPvE").unwrap() };
        let register_callback_fun=  unsafe { register_callback_fun.into_raw()};

        let create_program_ex: Symbol<LV_CreateProgramEx> = unsafe { lib.get(b"_Z18LV_CreateProgramExiiiii\0").unwrap() };
        let create_program_ex=  unsafe { create_program_ex.into_raw()};

        let add_program: Symbol<LV_AddProgram> = unsafe { lib.get(b"_Z13LV_AddProgramPviii\0").unwrap() };
        let add_program=  unsafe { add_program.into_raw()};

        let add_image_text_area: Symbol<LV_AddImageTextArea> = unsafe { lib.get(b"_Z19LV_AddImageTextAreaPviiP8AREARECTb\0").unwrap() };
        let add_image_text_area=  unsafe { add_image_text_area.into_raw()};

        let delete_program: Symbol<LV_DeleteProgram> = unsafe { lib.get(b"_Z16LV_DeleteProgramPv\0").unwrap() };
        let delete_program=  unsafe { delete_program.into_raw()};

        LedWrapper{
            lib: lib,
            init_server_fn: init_server_fnx,
            register_callback: register_callback_fun,
            create_program_ex: create_program_ex,
            add_program: add_program,
            add_image_text_area: add_image_text_area,
            delete_program:delete_program,
        }
    }
    pub fn init_server(&self, port: c_int) -> c_int {
        unsafe {
            (self.init_server_fn)(port)
        }
    }
    // pub fn setCallback(&self, callback: LedServerCallback)->c_int {
    //     unsafe {
    //         (self.register_callback)(Some(g_callback))
    //     }
    // }
}

unsafe extern "C" fn g_callback(Msg:c_int, wParam:c_int, lParam:std::ffi::c_void)->c_int
{
    println!("callback {} {}", Msg, wParam);
    return 0;
}
fn main() {
    // Load the shared library
    
    let wrapper = LedWrapper::new();
    
    // Call the function
    let port = 1234;
    let result = wrapper.init_server(port);
    if result != 0 {
        panic!("Failed to initialize server: {}", result);
    }
    println!("Server initialized on port {}", port);
    loop {
        std::thread::sleep(std::time::Duration::from_secs(100));
    }
}
