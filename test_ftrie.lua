local FT = require'ftrie'

local ft = FT.new()

ft:open("smalltest-GeoLite2-ASN-Blocks-IPv4.csv");

print"testing dotted"
local k,l = ft:lookup_dotted("74.62.72.72")
print(k, l )

print"testing trisulkey"
local k,l = ft:lookup_key("4A.3E.48.48")
print(k, l )

ft:close()

