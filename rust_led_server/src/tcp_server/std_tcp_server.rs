use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::{Arc, Mutex};
use std::thread;
#[derive(Clone, Debug)]
pub struct StdTcpServer {
    
    local_port: u16,
    remote_port: u16,
    clients: Arc<Mutex<Vec<TcpStream>>>,
}

impl StdTcpServer {

    pub fn New(local_port: u16, remote_port: u16) -> Self {
        Self {
            local_port,
            remote_port,
            clients: Arc::new(Mutex::new(Vec::<TcpStream>::new())),
        }
    }
    fn handle_client(&mut self,mut stream: TcpStream) {
        let mut buf = [0; 1024];
        let mut proxy = TcpStream::connect(("127.0.0.1", self.remote_port)).unwrap();
        loop {
            match stream.read(&mut buf) {
                Ok(0) => break,
                Ok(n) => {
                    let msg = &buf[..n];
                    proxy.write_all(msg).unwrap();                   
                }
                Err(e) => {
                    eprintln!("Error reading from socket: {}", e);
                    break;
                }
            }
            match proxy.read(&mut buf) {
                Ok(0) => break,
                Ok(n) => {
                    let msg = &buf[..n];
                    stream.write_all(msg).unwrap();
                }
                Err(e) => {
                    eprintln!("Error reading from proxy: {}", e);
                    break;
                }
            }
        }
        let mut clients = self.clients.lock().unwrap();
        clients.retain(|client| client.peer_addr().unwrap() != stream.peer_addr().unwrap());
        clients.iter().for_each(|client| {
            println!("discont>>>{:?}", client);
        })
    }
    pub fn getAllClients(&self)->String {
        let mut result ="".to_string();
        self.clients.lock().unwrap().iter().for_each(|client| {            
            let s2 =  client.peer_addr().unwrap().to_string();
            result+=format!("{}\n",s2).as_str();
        });
        result
    }
    pub fn displayClients(&self) {
        self.clients.lock().unwrap().iter().for_each(|client| {
            println!("{:?}", client);
        })
    }
    pub fn sendMessage(&mut self, message:  &[u8]) {
        let mut clients = self.clients.lock().unwrap();
        for client in clients.iter_mut() {
            client.write_all(message).unwrap();
        }
    }
    pub fn start_server(&mut self) -> std::io::Result<()> {

        let listener = TcpListener::bind(("0.0.0.0", self.local_port))?;
        println!("Listening on port {}", self.local_port);
        for stream in listener.incoming() {
            let stream = stream.unwrap();
            let addr = stream.peer_addr().unwrap();
            println!("New client connected: {}", addr);
            self.clients.lock().unwrap().push(stream.try_clone().unwrap());
            let mut self_clone = self.clone();
            thread::spawn(move || {
                self_clone.handle_client(stream);
                println!("Client disconnected: {}", addr);
            });
        }
        Ok(())
    }
}
