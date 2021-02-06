# Filtro de Borramento Adaptativo

## Pré-requisitos:
- OpenCV na versão 4x\
    Recomendo baixar da fonte: [Github OpenCV](https://github.com/opencv/opencv)

## Compilando e Executando o programa

> Não explicarei aqui como instalar e preparar o ambiente necessário, sinta-se a vontade para fazer 
> isso como achar melhor.

Usando o **make** basta fazer:

```
$ make
```
e o programa estará compilado se tudo estiver funcionando :P.

![Compilando](img/compilando.png)

## Executando

Para executar basta fazer:

```
./main input_image threshold
```

![Executando](img/executando.png)

Utilizei essa imagem como entrada:

![Entrada](1.jpg)

E um threshold de 50.

O programa gerou algumas imagens de saída que ajudam a visualizar as etapas realizadas pelo programa.

![Saídas](img/saidas.png)

Saídas do programa:

<img 
	title="Gradiente na direção X" 
    src="gx_q1.png" 
    width="150" 
    height="100"
/>
<img 
	title="Gradiente na direção Y" 
    src="gy_q1.png"
    width="150" 
    height="100"
/>
<img 
	title="Suavização Leve" 
    src="leve_suavizacao_q1.png"
    width="150" 
    height="100" 
/>
<img 
	title="Suavizaçao Forte" 
    src="borrada_q1.png"
    width="150" 
    height="100"
/>
<img 
	title="Entrada do Programa" 
    src="entrada_q1.png"
    width="150" 
    height="100"
/>
<img 
	title="Saída do Programa" 
    src="saida_q1.png"
    width="150" 
    height="100"
/>