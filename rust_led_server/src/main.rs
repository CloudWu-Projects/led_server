

use std::thread;
mod tcp_server;
use crate::tcp_server::std_tcp_server::StdTcpServer;
fn main(){
    let mut a = StdTcpServer::New(8080, 10008);
    let mut a_clone = a.clone();
    thread::spawn(move || {
        let mut input = String::new();
        loop {
            std::io::stdin().read_line(&mut input).unwrap();
            println!("{}>>{}",input.len(), input);
            if input.len() <3 {
               
                a_clone.displayClients();
                continue;
            }
            a_clone.sendMessage(input.as_bytes());
            
            input.clear();
        }
    });

    a.start_server();

}