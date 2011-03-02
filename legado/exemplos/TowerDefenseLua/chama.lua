sprite = 
{
	arquivo = "dados/chama.png",
	altura_quadro = 64,
	largura_quadro = 48
}

-- soma = true

function atualizar(id, frame, x, y, info) 

--    if soma == true then
--       x = x + 2
--       soma = false
--    else
--	   x = x - 2
--	   soma = true
--	end

	if info == 1 then
		y = y + 1
	end
	
	return frame, x, y, info
end 
