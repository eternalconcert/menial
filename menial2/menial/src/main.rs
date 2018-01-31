extern crate ansi_term;
extern crate serde_json;

use ansi_term::Colour::*;
use serde_json::{Value, Error};
use std::net::{TcpListener, TcpStream};
use std::env;
use std::process;
use std::thread;




struct Server {
    port: u16,
}


impl Server {

    fn run(&self) {
        let listener = TcpListener::bind(("127.0.0.1", self.port)).unwrap();
        for stream in listener.incoming() {
            self.handle_stream(stream.unwrap());
        }
    }

    fn handle_stream(&self, stream: TcpStream) {
        println!("Incoming connection on local address {:?} from: {:?}", stream.local_addr(), stream.peer_addr());
    }

}

fn parse_config(config_path: String) {
    println!("{:?}", config_path);
}

fn main() {

    if env::args().len() < 2 {
        println!("{}", Red.bold().paint("No config file path specified."));
        process::exit(1);
    }

    let args: Vec<String> = env::args().collect();
    let config_path = args[1].to_owned();
    parse_config(config_path);

    let mut threads = vec![];
    for i in 8080..8088 {
        let server: Server = Server { port: i };
        threads.push(
            thread::spawn(move|| {
                server.run();
            })
        )
    }

    for thread in threads {
        let _ = thread.join();
    }

}
