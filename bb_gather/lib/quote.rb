def quote(followers, depth)
	if depth > 0 then
		followers**depth + quote(followers,depth-1)
	else
		1
	end
end