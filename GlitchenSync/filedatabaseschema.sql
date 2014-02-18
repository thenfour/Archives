PRAGMA encoding = "UTF-8";

create table Settings
(
	[Name] text unique,
	[intValue] int,
	[stringValue] text
);

create table Files
(
	[relativePath] text primary key,
	[isDirectory] int,
	[MD5] blob,-- nullable
	[modifiedDate] blob,
	[size] blob-- UINT64 (machine-endian)
);

/*
	list of filenames that, if they do not exist,
	then do not consider them as "deleted". This is to account for the situation 
	where a subscriber is a SOURCE but not a DESTINATION. So the "complete"
	repository contains more files than this source has. Without this list,
	we would deduce that a bunch of files have been deleted from this subscriber
	because they are not there.
*/
create table UnexpectedFiles
(
	[subscriberID] text,-- if this could be a clustered primary key, this could be indexed. for now it's not so it will be SLOW.
	                    -- one option is to use 
	[relativePath] text
);
