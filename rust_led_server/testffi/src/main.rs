use libc::size_t;

extern crate libloading;

use std::env;
use libloading::{Library, Symbol};


type     LV_InitServer=extern "stdcall" fn(port :i32)->i32;

#[link(name="ledplayer7")]
extern "stdcall" {
    fn LV_InitServer(port :i32)->i32;
}

fn main() {
    println!("Hello, world!");
    {
        LV_InitServer(8089);
    }
    let library_path0 = "/home/admin/libledplayer7.so";
    let library_path1 = "F:\\Codes\\Cloud_wu\\led\\led_server\\lib\\dll\\lv_led_32.dll";
    let library_path2 = "F:\\Codes\\Cloud_wu\\led\\led_server\\lib\\dll\\lv_led_64.dll";
    let library_path3 = "F:\\Codes\\Cloud_wu\\led\\led_server\\lib\\dll\\lv_led_MBCS_32.dll";
    let library_path4 = "F:\\Codes\\Cloud_wu\\led\\led_server\\lib\\dll\\lv_led_MBCS_64.dll";
    let a=vec![library_path0,library_path1,library_path2,library_path3,library_path4];
    a.iter().for_each(|library_path|{
        println!("library_path {}",library_path);
        unsafe {
            let lib = Library::new(library_path);
            println!("lib {:?}",lib);
            let lib = lib.unwrap();
            let init_server: Symbol<LV_InitServer> = lib.get(b"LV_InitServer").unwrap();
            init_server(8089);
        }
    });
}
