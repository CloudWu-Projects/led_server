use std::thread;
mod tcp_server;
use crate::tcp_server::std_tcp_server::StdTcpServer;
//use tiny_http::{Response, Server};

#[macro_use]
extern crate rouille;

use rouille::Request;
use rouille::Response;
fn main() {
    println!("[ Auto FFI bindgins call demo ]\n");

}
fn startAllServer(){

    let mut a = StdTcpServer::New(10008, 10009);
    let mut a_clone = a.clone();
    thread::spawn(move || {
        a_clone.start_server();
        let mut input = String::new();
        loop {
            std::io::stdin().read_line(&mut input).unwrap();
            println!("{}>>{}", input.len(), input);
            if input.len() < 3 {
                a_clone.displayClients();
                continue;
            }
            a_clone.sendMessage(input.as_bytes());

            input.clear();
        }
    });
    //let rocket=  rocket::build().mount("/", routes![index]);

    rouille::start_server("0.0.0.0:8000", move |request| {
        let response = note_routes(request, &a);
        response
    });
    
    println!("Hello, world!");
    //maybe useful
    //https://github.com/tomaka/rouille/blob/master/examples/database.rs  
}

fn note_routes(request:&Request ,tcpServer:&StdTcpServer) -> Response {
    router!(request, 
        (GET) (/) => {           
            let _rtext = tcpServer.getAllClients().to_string();
            let mut ab = String::from("hello world");
            ab.push_str(_rtext.as_str());
            Response::text(ab)
        },    
        (GET) (/setparknum) => {
            let key = request.get_param("key");
            println!("key {:?}",key);

            let _rtext = tcpServer.getAllClients().to_string();
            let mut ab = String::from("hello world");
            ab.push_str(_rtext.as_str());
            Response::text(ab)
        },    
        
        _ => Response::empty_404()
    )
}