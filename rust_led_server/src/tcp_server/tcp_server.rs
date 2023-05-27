use std::collections::HashMap;
use std::collections::HashSet;
use std::net::SocketAddr;
use std::sync::{Arc, Mutex};
use tokio::io::{self, AsyncReadExt, AsyncWriteExt};
use tokio::net::{TcpListener, TcpStream};
pub struct TcpServer {
    local_port: u16,
    remote_port: u16,
    clients2: Arc<Mutex<HashMap<SocketAddr, TcpStream>>>,
    clients: Arc<Mutex<HashSet<SocketAddr>>>,
}

fn display_clients2(db: &HashMap<SocketAddr, TcpStream>) {
    db.iter().for_each(|client| {
        println!("{}", client.0);
    })
}

fn display_clients(db: &HashSet<SocketAddr>) {
    db.iter().for_each(|client| {
        println!("{}", client);
    })
}
impl TcpServer {
    pub fn new(local_port: u16, remote_port: u16) -> Self {
        Self {
            local_port,
            remote_port,
            clients: Arc::new(Mutex::new(HashSet::new())),
            clients2: Arc::new(Mutex::new(HashMap::new())),
        }
    }
    pub fn sendMessage(&mut self, message: String) {
        println!("{}", message);
        {
           
        }
    }
    pub async fn start_tcp_server(&mut self) {
        let listener = TcpListener::bind(("0.0.0.0", self.local_port))
            .await
            .unwrap();

        println!("Proxy listening on port {}", self.local_port);
        let g_remote_addr = format!("127.0.0.1:{}", self.remote_port);

        loop {
            let (client_stream, client_addr) = listener.accept().await.unwrap();

            let db = self.clients.clone();
            {
                let mut db = db.lock().unwrap();
                db.insert(client_addr);
                display_clients(&db);
            }
            println!("New connection from {}", client_addr);
            let target_addr = g_remote_addr.clone();

            tokio::spawn(async move {
                proxy(client_stream, client_addr, target_addr).await;

                {
                    let mut db = db.lock().unwrap();
                    println!("after removed  {}", client_addr);
                    db.remove(&client_addr);
                    display_clients(&db);
                }
            });
        }
    }

    pub async fn start_server(local_port: u16, remote_port: u16) {
        TcpServer::new(local_port, remote_port)
            .start_tcp_server()
            .await;
    }
}

async fn proxy(client_stream: TcpStream, client_addr: SocketAddr, target_addr: String) {
    let server_stream = TcpStream::connect(target_addr).await.unwrap();

    let (mut client_reader, mut client_writer): (
        io::ReadHalf<TcpStream>,
        io::WriteHalf<TcpStream>,
    ) = io::split(client_stream);
    let (mut server_reader, mut server_writer) = io::split(server_stream);

    let client_to_server = tokio::spawn(async move {
        io::copy(&mut client_reader, &mut server_writer)
            .await
            .unwrap();
    });

    let server_to_client = tokio::spawn(async move {
        io::copy(&mut server_reader, &mut client_writer)
            .await
            .unwrap();
    });

    tokio::select! {
        _ = client_to_server => {
            println!("Client -> Server transfer complete");
        }
        _ = server_to_client => {
            println!("Server -> Client transfer complete");
        }
    }
}
