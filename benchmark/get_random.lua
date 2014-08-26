arguments = {}

init = function(args)
    wrk.init(args)
    arguments = args
end

request = function()
    pipeline_length = tonumber(arguments[1]) or 1
    local r = {}

    for i=1,pipeline_length do
        h = {
            keyspace = "database",
            key      = string.format("%010d", math.random(0, 1000000))
        }
        r[i] = wrk.format("GET", "/", h)
    end

    req = table.concat(r)

    return req
end
