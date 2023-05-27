mod tcp_server;
use std::{thread, time};

use tokio::join;

use crate::tcp_server::TcpServer;
#[tokio::main]
async fn main() {
    let local_port = 10008;
    let remote_port = 10009;
    let mut tcpS = TcpServer::new(local_port, remote_port);

    let a = tcpS.start_tcp_server();

    let loopthread = thread::spawn(move || {
        let mut acount = 0;
        loop {
            thread::sleep(time::Duration::from_secs(1));
            println!("{:?}", acount);

            acount = acount + 1;
        }
    });

    join!(a);
    println!("over !!!!!!!!!!!")
}
