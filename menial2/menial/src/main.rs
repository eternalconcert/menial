use std::net::{TcpListener, TcpStream};
use std::thread;

fn handle_stream(stream: TcpStream) {
    println!("Incoming connection on local address {:?} from: {:?}", stream.local_addr(), stream.peer_addr());
}

struct Server {
    port: u16,
}

impl Server {

    fn run(&self) {
        let listener = TcpListener::bind(("127.0.0.1", self.port)).unwrap();
        for stream in listener.incoming() {
            handle_stream(stream.unwrap());
        }
    }

}

fn main() {

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
