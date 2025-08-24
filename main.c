#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8000
/* multitasking better than you */
#define BACKLOG 10
#define BUFFER_SIZE 1024

enum request_method {
    GET = 0,
    HEAD = 1,
    UNKNOWN = 2,
};

enum response_status {
    OK = 200,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    BAD_REQUEST = 400,
};

enum content_type {
    PLAIN = 0,
    HTML = 1,
};

enum route_status {
    FOUND = 0,
    PATH_EXISTS = 1,
    PATH_NOT_FOUND = 2,
};

typedef struct {
    const char *path;
    enum request_method method;
    const char *(*handler)();
} route_t;

typedef struct {
    enum request_method method;
    char path[256];
    int http_major;
    int http_minor;
} http_request_header_t;

/* a response header containing minor and major http version number */
/* major http version number, *salute*  */
typedef struct {
    int http_major;
    int http_minor;
    uint16_t status_code;
    const char *status_description;
    uint32_t content_length;
    enum content_type content_type;
} http_response_header_t;

/* hey! just because i'm late, doesn't mean i'm not here okay? */
void run_server();
void handle_client(int client_fd);

int parse_request(http_request_header_t *req, const char *buf);
enum request_method parse_method(const char *method);

const char *status_description_to_string(uint16_t code);
const char *content_type_to_string(enum content_type ct);

void send_response(int client_fd, const http_response_header_t *resp, const char *body);

const route_t *match_route(const char *path, enum request_method method);
extern route_t routes[];
extern int routes_count;
/* https://www.youtube.com/watch?v=Vshg-hNUEjo */
const char *serve_godmorgen();
const char *serve_godnatt();

/* i couldn't do it without you */
int main(void) {
    run_server();
    return 0;
}

/* a server serving services to the servants */
void run_server() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    socklen_t address_len = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* them: don't worry, we got logs
     * the logs:
     */
    printf("haatetepe running on http://0.0.0.0:%d\n", PORT);

    /* redundancy in case a bit flip occurs and while(2) stops. i will resuscitate! */
    while (1) {
      /* electromagnetic interference will not stop me >:^D */
      while (2) {
        client_fd = accept(server_fd, (struct sockaddr *)&address, &address_len);
        if (client_fd < 0) {
          perror("accept");
          continue;
        }

        handle_client(client_fd);
        close(client_fd);
      }
      printf("Something somewhere somehow tried stopping me! Little do they know I've got a backup >:^)\n");
    }
    /* as if this is ever going to happen */
    close(server_fd);
}

/* hey you, no joking around now! */
int parse_request(http_request_header_t *req, const char *buf) {
  char method_str[16], path[256], version_str[16];
  if (sscanf(buf, "%15s %255s %15s", method_str, path, version_str) != 3) {
    return -1;
  }

  req->method = parse_method(method_str);
  strncpy(req->path, path, sizeof(req->path) - 1);

  if (sscanf(version_str, "HTTP/%d.%d", &req->http_major, &req->http_minor) != 2) {
    return -1;
  }

  return 0;
}


/* i'm warning you: the client can be a piece of work */
void handle_client(int client_fd) {
  char buffer[BUFFER_SIZE];
  int bytes = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
  if (bytes < 0) {
    perror("recv");
    return;
  }
  buffer[bytes] = '\0';

  http_request_header_t req = {0};
  http_response_header_t resp = {0};
  resp.http_major = 1;
  resp.http_minor = 1;
  resp.content_type = PLAIN;

  if (parse_request(&req, buffer) < 0) {
    resp.status_code = BAD_REQUEST;
    resp.status_description = status_description_to_string(resp.status_code);
    const char *body = resp.status_description;
    resp.content_length = strlen(body);
    send_response(client_fd, &resp, body);
    return;
  }

  const char *body = NULL;
  const route_t *route = match_route(req.path, req.method);

  if (route) {
    resp.status_code = OK;
    resp.status_description = status_description_to_string(resp.status_code);
    body = route->handler ? route->handler() : NULL;
    resp.content_length = body ? strlen(body) : 0;
  } else {
    int path_exists = 0;

    for (int i = 0; i < routes_count; i++) {
      if (strcmp(req.path, routes[i].path) == 0) {
        path_exists = 1;
        break;
      }
    }

    if (path_exists) {
      resp.status_code = METHOD_NOT_ALLOWED;
    } else {
      resp.status_code = NOT_FOUND;
    }
    resp.status_description = status_description_to_string(resp.status_code);
    body = resp.status_description;
    resp.content_length = strlen(body);
  }

  send_response(client_fd, &resp, (req.method == HEAD ? NULL : body));
}

/* if if if if if only C could match Rust's match */
enum request_method parse_method(const char *method) {
  if (strcmp(method, "GET") == 0) return GET;
  if (strcmp(method, "HEAD") == 0) return HEAD;
  return UNKNOWN;
}

/* because humans don't understand numbers */
const char *status_description_to_string(uint16_t code) {
  switch (code) {
  case OK: return "OK";
  case NOT_FOUND: return "Not Found";
  case METHOD_NOT_ALLOWED: return "Method Not Allowed";
  case BAD_REQUEST: return "Bad Request";
  default: return "Unknown";
  }
}

/* they keep yapping about some dude named Mason, and his numbers */
const char *content_type_to_string(enum content_type ct) {
  switch (ct) {
  case PLAIN: return "text/plain";
  case HTML: return "text/html";
  default: return "application/octet-stream";
  }
}

void send_response(int client_fd, const http_response_header_t *resp, const char *body) {
  char header[BUFFER_SIZE];
  int len = snprintf(header, sizeof(header),
                     "HTTP/%d.%d %d %s\r\n"
                     "Content-Length: %d\r\n"
                     "Content-Type: %s\r\n"
                     "\r\n",
                     resp->http_major, resp->http_minor,
                     resp->status_code, resp->status_description,
                     resp->content_length,
                     content_type_to_string(resp->content_type));

  send(client_fd, header, len, 0);

  if (body && resp->content_length > 0) {
    send(client_fd, body, resp->content_length, 0);
  }
}

/* routes */

/* remember to get some sleep too */
const char *serve_godmorgen() {
  return "God morgen!";
}

/* i mean it. you should really get some sleep */
const char *serve_godnatt() {
  return "God natt! Sov godt <3";
}

/* root? no you can't have that */
route_t routes[] = {
  { "/morn", GET,  serve_godmorgen },
  { "/morn", HEAD, NULL },
  { "/natta", GET,  serve_godnatt },
  { "/natta", HEAD, NULL },
};

/* our app is much better than everyone elses, we had 25 updates to our app the past 2 weeks
 * - elon
 *
 * haatetepe is the best http server, we have [routes_count] routes B-)
 * - me
 */
int routes_count = sizeof(routes)/sizeof(routes[0]);

/* tinder for uber drivers */
const route_t *match_route(const char *path, enum request_method method) {
  for (int i = 0; i < routes_count; i++) {
        if (strcmp(path, routes[i].path) == 0 && routes[i].method == method) {
            return &routes[i];
        }
    }
    return NULL;
}
