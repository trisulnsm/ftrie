-- ftrie.lua
-- 
-- LuaJIT interface to ftrie GeoIP lookup 
-- 
-- Main use case : Used in Trisul Network Analytics LUA scripts   https://trisul.org
--
-- *** INCOMPLETE ***
-- 
local ffi=require'ffi'
local L=ffi.load'libftrie.so'

-- 
-- From LevelDB c.h API - we are only using a basic subset 
-- 
ffi.cdef [[
	typedef void 	GeoIP;

	GeoIP * 		GeoIP_open(const char * path, uint32_t flags);
	void			GeoIP_delete(GeoIP * pdb);
	bool 		    GeoIP_by_ipnum(GeoIP * GeoIP_Handle, uint32_t ipnum, const char ** key, const char ** label);

]]

local sleveldb = {

  -- return true or false,errmsg 
  open=function(tbl, dbpath)

  		local db = L.GeoIP_open(dbpath, 0);
		if db then
			tbl.db=db
			return true, db
		else
			tbl.db=nil
			return false, "error opening db"
		end
  end,

  -- key in trisul format 
  lookup=function(tbl, ipv4_key)

  end,


  close=function(tbl)
  	L.GeoIP_delete(tbl.db)
  end

      return nil 
    else 
      return ffi.string(val,readlen[0])
    end 
  end,



  -- put a KV 
  put=function(tbl,k,v)

    L.leveldb_put( tbl._db, tbl.write_opts, k,#k, v, #v, tbl.errmsg)
    if tbl.errmsg[0] == nil then
      return true, ""
    else
      local emsg = ffi.string(tbl.errmsg[0]);
      L.leveldb_free( tbl.errmsg[0] ) 
      return false, emsg
    end

  end,

  -- put bulk
  -- uses writeBatch to write out the table (k,v) 
  put_table=function(tbl, keyval_table) 

  	local wbatch = L.leveldb_writebatch_create()

  	for k,v in pairs(keyval_table)
  	do 
  		local ks= tostring(k)
  		local vs= tostring(v) 
  		L.leveldb_writebatch_put( wbatch, ks,#ks,vs,#vs)
  	end 

    L.leveldb_write( tbl._db, tbl.write_opts, wbatch,  tbl.errmsg)
    L.leveldb_writebatch_destroy(wbatch)

    if tbl.errmsg[0] ~= nil  
    then 
      local emsg = ffi.string(tbl.errmsg[0]);
      L.leveldb_free( tbl.errmsg[0] ) 
      print(emsg) 
      return false, emsg
    end
  end, 

  -- delete a key 
  delete=function(tbl,k)
    L.leveldb_delete( tbl._db, tbl.write_opts, k,#k ,tbl.errmsg)
    if tbl.errmsg[0] == nil then
      return true
    else
      local emsg = ffi.string(tbl.errmsg[0]);
      L.leveldb_free( tbl.errmsg[0] ) 
      return false, emsg
    end
  end,
  
  -- iterator 
  create_iterator=function(tbl)
    return Iterator.new(tbl._db)
  end,

  -- get keys lexicographically above and below the key
  -- return k0,v0,k1,v1  - if exact match, k0,v0,k0,v0
  -- nils if invalid 
  get_bounds=function(tbl,iterator, key)

      iterator:seek_to(key)
      if not iterator:valid()  then return nil end 

      local k0,v0 = iterator:key_value()
      if k0==key then 
        return k0,v0,k0,v0
      end

      iterator:iter_prev()
      if not iterator:valid()  then return nil end 
      local k1,v1 = iterator:key_value()
      return k0,v0,k1,v1
  end,

  -- upper match
  -- 
  upper=function(tbl,iterator,key,fn_match)
    iterator:seek_to(key)
    if not iterator:valid()  then return nil end 

    local k0,v0 = iterator:key_value()

  	if fn_match == nil or fn_match(k0,key) then 
  		return k0,v0
  	else
  		iterator:iter_prev()
  		if not iterator:valid()  then return nil end 
  		return iterator:key_value()
  	end
  end,

  -- lower match
  --
  lower=function(tbl,iterator,key, fn_match)
    iterator:seek_to(key)
    if not iterator:valid()  then return nil end 

    local k0,v0 = iterator:key_value()

  	if fn_match == nil or fn_match(k0,key) then 
  		return k0,v0
  	else
  		iterator:iter_next()
  		if not iterator:valid()  then return nil end 
  		return iterator:key_value()
  	end
  end,

  -- dump the whole database 
  dump=function(tbl)

    print("----Dumping----")
    local iter=tbl.create_iterator(tbl)
    iter:seek_to_first()
    while iter:valid() do 
      local k,v = iter:key_value()
      print(k.."="..v)
      iter:iter_next()
    end 
    iter:destroy()
    print("End----")

  end
}

local LevelDB   = { 
  new = function( ) 
    return setmetatable(  {
      _db = nil ,
      write_opts=nil,
      read_opts=nil,
      errmsg=nil,
      owner=true,
    }, { __index = sleveldb} )
  end
} 

return LevelDB 

