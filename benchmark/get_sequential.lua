counter = 0
arguments = {}

init = function(args)
    wrk.init(args)
    arguments = args
end

request = function()
    pipeline_length = tonumber(arguments[1]) or 1
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

    return req
end
