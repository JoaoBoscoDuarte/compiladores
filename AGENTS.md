# AGENTS.md

Guia para agentes de IA (e humanos) trabalhando neste repositório. É o
projeto da disciplina de Construção de Compiladores: um compilador para a
linguagem EC1 (Expressões Constantes 1), construído de forma incremental,
uma atividade por vez.

## Estrutura (leia antes de tocar em qualquer pasta)

- `compiler/` — **o compilador atual, sempre atualizado**. Reúne o estado
  mais recente de todas as atividades já concluídas. É o que vale pra
  entregas tipo "Marco" (checkpoints que avaliam o projeto acumulado) e pra
  seguir desenvolvendo as próximas atividades. Tem seu próprio
  `README.md`/`Makefile` com instruções de build e execução — leia-os em
  vez de assumir.
- `atvN/` (`atv2`, `atv3`, `atv4`, `atv5`, ...) — **cópias autocontidas da
  entrega de cada atividade**, no estado em que foram submetidas. Cada uma
  tem seu próprio `README.md`/`Makefile`/`docs/<enunciado>.pdf`. São
  registro histórico — só edite uma `atvN/` já entregue se for pra
  corrigir um erro real nela (como aconteceu uma vez, ver "Build e teste
  sempre" abaixo), não pra adicionar funcionalidade nova.
- Raiz: `README.md` lista as pastas; `.gitignore` cobre os artefatos de
  build (`bin/`, `obj/`, `asm/`) de cada pasta — ao adicionar uma `atvN/`
  nova, adicione as três linhas correspondentes lá.

## Fluxo ao terminar uma atividade nova

Quando uma `atvN/` nova for concluída, ela deve ser **consolidada em
`compiler/`** — não existe passo automático pra isso, é trabalho manual:

1. Compare `atvN/include` e `atvN/src` com o que já existe em `compiler/`
   pra identificar o que é novo ou mudou.
2. Copie/mescle os arquivos novos para `compiler/include` e `compiler/src`,
   mantendo os nomes com prefixo da linguagem (ver convenção abaixo).
3. Atualize `compiler/Makefile` (novo `.o` na lista de objetos do binário
   certo) e `compiler/README.md` (capacidades novas, flags novas, etc).
4. **Rebuilde e rode os testes de `compiler/` inteiro antes de dar
   qualquer coisa como pronta** — ver seção seguinte.
5. Se a atividade nova tornar uma linguagem/binário anterior irrelevante
   pro escopo atual (ex: um Marco que só cobre certas atividades), é
   válido remover esse binário de `compiler/` — mas isso é uma decisão do
   usuário, não assuma sozinho.

## Build e teste sempre, nunca só "ler o código"

Cada pasta (`compiler/`, `atvN/`) tem seu próprio `make` e script de teste
(`tests/run_*.sh` ou `tests/*.ec1` + script). Depois de qualquer mudança em
`include/`, `src/` ou `tests/`, rode `make clean && make` e o script de
teste da pasta antes de considerar a mudança concluída — **não confie só
em inspeção visual do conteúdo dos arquivos**.

Isso não é regra genérica de boa prática: já aconteceu nesse repo uma
entrega (`atv5/`) cujo conteúdo de `include/`, `src/` e `tests/` tinha sido
trocado entre arquivos (nome certo, conteúdo errado) — o projeto
simplesmente não compilava, e isso só ficou óbvio rodando `make`. Rodar
antes e depois de editar é a única forma barata de pegar isso cedo.

## Convenções da linguagem EC1

- Gramática: `<expressao> ::= <num> | '(' <exp> <op> <exp> ')'`, sem
  precedência (tudo entre parênteses). Ver `docs/CC-Atividade0N*.pdf` de
  cada atividade pro enunciado original.
- Arquivos da linguagem EC1 usam prefixo `ec1_` (`ec1_ast.hpp`,
  `ec1_lexer.*`, `ec1_parser.*`, `ec1_codegen.*`) justamente pra poder
  conviver em `compiler/src`/`compiler/include` com arquivos de outras
  linguagens da disciplina sem colidir nome (ex: `parser.cpp` genérico de
  uma atividade anterior). Siga o padrão ao adicionar uma etapa nova.
- Geração de código usa esquema de tradução com pilha (`push`/`pop` em
  `%rax`/`%rbx`, `%rdx` só na divisão) — ver comentários em
  `*_codegen.cpp` antes de alterar.
- **Limitação conhecida, não é bug**: a linguagem não define comportamento
  de divisão por zero. O interpretador (`eval()`) e o assembly gerado
  (`idiv`) ambos têm comportamento indefinido/crash (`SIGFPE`) nesse caso —
  isso é esperado e documentado nos READMEs, não precisa de correção a
  menos que o enunciado de uma atividade futura passe a exigir.

## Git e autoria

- **Nunca** adicione trailers `Co-Authored-By: Claude` ou links de sessão
  nos commits deste repo — o usuário pediu explicitamente para não
  aparecer Claude como colaborador/coautor. Autor e committer do commit
  devem ser a identidade do aluno configurada localmente (`git config
  user.name`/`user.email` já aponta pra isso neste checkout; se for um
  checkout novo, configure antes do primeiro commit).
- Peça confirmação antes de dar merge/push em `main` — o fluxo normal é
  trabalhar num branch e só levar pra `main` quando combinado (já
  aconteceu de um merge parcial via PR deixar `main` desatualizado em
  relação ao branch; sempre confira `git log --oneline --graph --all`
  depois de qualquer PR/merge pra garantir que não sobrou nada pra trás).

## Avaliações "Marco"

O professor usa checkpoints chamados "Marco N", cada um cobrindo um
subconjunto das atividades (ex: Marco 1 = Atividades 4 e 5) e exigindo um
vídeo curto (~5min) explicando conceitos + código + processo de
desenvolvimento — não exige nada sobre estrutura de pastas/arquivos. Se
pedirem ajuda com um Marco novo, primeiro leia o PDF do marco em anexo
(geralmente carregado na conversa, não fica no repo) pra saber exatamente
o escopo e os critérios antes de montar qualquer material de apresentação.
