

use std::thread;
mod tcp_server;
use crate::tcp_server::std_tcp_server::StdTcpServer;

#[macro_use] extern crate rocket;

#[get("/")]
fn index() -> &'static str {
    "Hello, world!"
}
use rocket::{Request, Data};
use rocket::route::{Handler, Route, Outcome};
use rocket::http::Method;
#[derive(Copy, Clone)]
enum Kind {
    Simple,
    Intermediate,
    Complex,
}

#[derive(Clone)]
struct CustomHandler(Kind);


#[rocket::async_trait]
impl Handler for CustomHandler {
    async fn handle<'r>(&self, req: &'r Request<'_>, data: Data<'r>) -> Outcome<'r> {
        match self.0 {
            Kind::Simple => Outcome::from(req, "simple"),
            Kind::Intermediate => Outcome::from(req, "intermediate"),
            Kind::Complex => Outcome::from(req, "complex"),
        }
    }
}

impl Into<Vec<Route>> for CustomHandler {
    fn into(self) -> Vec<Route> {
        vec![Route::new(Method::Get, "/", self)]
    }
}


#[rocket::main]
async fn main() {
    let mut a = StdTcpServer::New(8080, 10008);
    let mut a_clone = a.clone();
    thread::spawn(move || {
        a_clone.start_server();
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
    //let rocket=  rocket::build().mount("/", routes![index]);
    let rocket =  rocket::build().mount("/", CustomHandler(Kind::Simple))
    .mount("/a", CustomHandler(Kind::Intermediate))
    .mount("/b", CustomHandler(Kind::Complex));
    rocket.launch().await;
     println!("Hello, world!");
}