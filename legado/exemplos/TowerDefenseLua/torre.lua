sprite = 
{
	arquivo = "dados/torre.png",
	altura_quadro = 0,
	largura_quadro = 0
}

soma = true

function atualizar(id, frame, x, y, info) 

	if info == 1 then
		y = y + 1
		if soma == true then
		    x = x + 3
		    soma = false 
		else
			x = x - 3
			soma = true
		end
	end
	
	
	return frame, x, y, info
end 
