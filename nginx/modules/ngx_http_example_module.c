#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
static char *ngx_http_larva(ngx_conf_t *cf, void *post, void *data);

static ngx_conf_post_handler_pt ngx_http_larva_p = ngx_http_larva;


/*
 * The structure will hold the value of the
 * module directive larva
 */
typedef struct
{
    ngx_str_t name;
} ngx_http_larva_loc_conf_t;


/* The function which initializes memory for the module configuration
structure
 */
static void *
ngx_http_larva_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_larva_loc_conf_t *conf;
    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_larva_loc_conf_t));
    if (conf == NULL)
    {
        return NULL;
    }

    return conf;
}

/*
 * The command array or array, which holds one sub-array for each
module
 * directive along with a function which validates the value of the
 * directive and also initializes the main handler of this module
 */
static ngx_command_t ngx_http_larva_commands[] = {
    {ngx_string("larva"),
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_str_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_larva_loc_conf_t, name),
     &ngx_http_larva_p},
    ngx_null_command};


static ngx_str_t larva_string;

/*
 * The module context has hooks, here we have a hook
 * for creating location configuration.
 */
static ngx_http_module_t ngx_http_larva_module_ctx = {
    NULL,                           /* Pre-configuration. */
    NULL,                           /* Post-configuration. */
    NULL,                           /* Create main configuration. */
    NULL,                           /* Init main configuration. */
    NULL,                           /* Create server configuration. */
    NULL,                           /* Merge server configuration. */
    ngx_http_larva_create_loc_conf, /* Create location configuration. */
    NULL                            /* Merge location configuration .*/
};


/*
 * The module which binds the context and commands.
 */
ngx_module_t ngx_http_larva_module = {
    NGX_MODULE_V1,
    &ngx_http_larva_module_ctx, /* Module context. */
    ngx_http_larva_commands,    /* Module directives. */
    NGX_HTTP_MODULE,            /* Module type. */
    NULL,                       /* Init master. */
    NULL,                       /* Init module.*/
    NULL,                       /* Init process. */
    NULL,                       /* Init thread. */
    NULL,                       /* Exit thread. */
    NULL,                       /* Exit process. */
    NULL,                       /* Exit master. */
    NGX_MODULE_V1_PADDING};

/*
 * Main handler function of the module.
 */
static ngx_int_t
ngx_http_larva_handler(ngx_http_request_t *r)
{
    ngx_int_t rc;
    ngx_buf_t *b;
    ngx_chain_t out;

    /* We response to 'GET' and 'HEAD' requests only.*/
    if (!(r->method & (NGX_HTTP_GET | NGX_HTTP_HEAD)))
    {
        return NGX_HTTP_NOT_ALLOWED;
    }

    /* Discard request body, since we don't need it here.*/
    rc = ngx_http_discard_request_body(r);
    if (rc != NGX_OK)
    {
        return rc;
    }

    /* Set the 'Content-type' header. */
    r->headers_out.content_type_len = sizeof("text/html") - 1;
    r->headers_out.content_type.len = sizeof("text/html") - 1;
    r->headers_out.content_type.data = (u_char *)"text/html";

    /* Send the header only, if the request type is http 'HEAD'. */
    if (r->method == NGX_HTTP_HEAD)
    {
        r->headers_out.status = NGX_HTTP_OK;
        r->headers_out.content_length_n = larva_string.len;
        return ngx_http_send_header(r);
    }

    /* Allocate a buffer for your response body. */
    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    if (b == NULL)
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    /* Attach this buffer to the buffer chain. */
    out.buf = b;
    out.next = NULL;

    /* Adjust the pointers of the buffer. */
    b->pos = larva_string.data;
    b->last = larva_string.data + larva_string.len;
    b->memory = 1;   /* this buffer is in memory */
    b->last_buf = 1; /* this is the last buffer in the buffer chain
                      */

    /* Set the status line. */
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = larva_string.len;

    /* Send the headers of your response. */
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only)
    {
        return rc;
    }

    /* Send the buffer chain of your response. */
    return ngx_http_output_filter(r, &out);
}



/*
 * Function for the directive larva, it validates its value
 * and copies it to a static variable to be printed later.
 */
static char *
ngx_http_larva(ngx_conf_t *cf, void *post, void *data)
{
    ngx_http_core_loc_conf_t *clcf;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_larva_handler;
    ngx_str_t *name = data; // i.e., first field of ngx_http_larva_loc_conf_t/
    
    if (ngx_strcmp(name->data, "") == 0)
    {
        return NGX_CONF_ERROR;
    }

    larva_string.data = name->data;
    larva_string.len = ngx_strlen(larva_string.data);
    
    return NGX_CONF_OK;
}