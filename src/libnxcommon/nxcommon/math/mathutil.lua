function mix(x, y, a)
    return x*(1-a) + y*a
end

function clamp(x, minv, maxv)
    return math.max(math.min(x, maxv), minv)
end

function smoothstep(a, b, x)
    x = clamp((x-a) / (b-a), 0, 1)
    return x*x * (3 - 2*x)
end
