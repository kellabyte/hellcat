-- example dynamic request script which demonstrates changing
-- the request path and a header for each request
-------------------------------------------------------------
-- NOTE: each wrk thread has an independent Lua scripting
-- context and thus there will be one counter per thread

counter = 0

init = function(args)
    wrk.init(args)

    pipeline_length = tonumber(args[1]) or 1
    local r = {}

    for i=1,pipeline_length do
        counter = counter + 1
        padded_value = string.format("%010d", counter)
        
        h = {
            keyspace = "database",
            key      = padded_value
        }
        r[i] = wrk.format("GET", "/", h)
    end

    req = table.concat(r)
end

request = function()
    return req
end
