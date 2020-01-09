SCC-0215 - Organização de Arquivos.
===================================

Esse projeto foi desenvolvidos atendendo aos requisitos dos trabalhos praticos da disciplina da USP de codigo SCC-0215.
Para uma melhor descrição sobre entradas, saidas e funcionamento do trabalho leia as [especificações](ProjDesc).

Cada arquivo na pasta de descrições especifica uma parte do projeto:
* Funcionalidades 1-3: [T1](ProjDesc/T1.pdf).
* Funcionalidades 4-6: [T2](ProjDesc/T2.pdf).
* Funcionalidades 7-9: [T3](ProjDesc/T3.pdf).
* Funcionalidades 10-14: [T4](ProjDesc/T4.pdf).

Como Usar:
----------

Para compilar voce pode simplesmente usar:
```shell
    > make
```
Para rodar use:
```shell
    > make run case=n
```
n especifica algum caso teste contido na pasta [Test/Cases](Test/Cases/).
Se n for algum numero no intervalo [1, 64], então o programa sera executado com o caso teste especificado como entrada. 

Ou seja, se usar em um terminal:
```shell
    > make run case=13
```
O programa será executado usando o arquivo [13.in](Test/Cases/13.in) como entrada.

Alternativamente, se n não pertencer ao intervalo [1, 64] o programa ira rodar normalmente, sem entrada nenhuma, permitindo que o usuário insira uma entrada customizada.

Ou seja, usar:
```shell
    > make run case=99
```
Ou:
```shell
    > make run
```
Irá iniciar o programa de modo que ele esperará pela entrada manual das informações.

Suporte:
-------

Se estiver tendo qualquer problema, sugestões ou correções para qualquer parte do projeto, me avise.<br>
Você pode me contatar em edpirro.ep@gmail.com.

Licensa:
-------
Esse projeto é licensiado sob a [licensa MIT](LICENSE).

<sub><sup>
*Os direitos sobre as listas  de exercicio ([aqui](ProjDesc)) são da professora e monitores da disciplina (creditos dentro do arquivo).
</sup></sub>

Reposiório:
----------

[Voltar à página principal do projeto.](https://github.com/EdPirro/USP-SCC0215)