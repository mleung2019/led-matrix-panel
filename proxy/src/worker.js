export default {
  async fetch(request, env, ctx) {
    const url = new URL(request.url);

    const upstream = "https://multi-use-led-matrix-64x64.onrender.com";
    const targetURL = upstream + url.pathname + url.search;

    // Copy headers
    const newHeaders = new Headers(request.headers);

    // Remove Cloudflare headers
    newHeaders.delete("cf-connecting-ip");
    newHeaders.delete("cf-ray");
    newHeaders.delete("x-forwarded-proto");
    newHeaders.delete("x-forwarded-for");

    // Read body (only for non-GET/HEAD methods)
    let body = null;
    if (request.method !== "GET" && request.method !== "HEAD") {
      body = await request.arrayBuffer();
    }

    // Forward request to Render
    const upstreamRes = await fetch(targetURL, {
      method: request.method,
      headers: newHeaders,
      body: body,
      redirect: "manual",
    });

    // Copy response headers
    const respHeaders = new Headers(upstreamRes.headers);
    respHeaders.delete("transfer-encoding");

    return new Response(upstreamRes.body, {
      status: upstreamRes.status,
      statusText: upstreamRes.statusText,
      headers: respHeaders,
    });
  },
};
