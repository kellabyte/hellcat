-- example dynamic request script which demonstrates changing
-- the request path and a header for each request
-------------------------------------------------------------
-- NOTE: each wrk thread has an independent Lua scripting
-- context and thus there will be one counter per thread

init = function(args)
    wrk.init(args)

    pipeline_length = tonumber(args[1]) or 1
    local r = {}

    for i=1,pipeline_length do
        h = {
            key = math.random(0, 1000000)
        }
        r[i] = wrk.format("GET", "/", h)
    end

    req = table.concat(r)
end

request = function()
    return req
end