use std::thread;
mod tcp_server;
use crate::tcp_server::std_tcp_server::StdTcpServer;
use tiny_http::{Response, Server};

fn main() {
    let mut a = StdTcpServer::New(8080, 10008);
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

    let server = Server::http("0.0.0.0:8000").unwrap();

    for request in server.incoming_requests() {
        println!(
            "received request! method: {:?}, url: {:?}, headers: {:?}",
            request.method(),
            request.url(),
            request.headers()
        );
        let response = Response::from_string("hello world");
        request.respond(response);
    }

    println!("Hello, world!");
    //maybe useful
    //https://github.com/tomaka/rouille/blob/master/examples/database.rs  
}
