# CLAUDE.md

Leia [`AGENTS.md`](AGENTS.md) primeiro — é a referência completa deste
repositório (estrutura, fluxo de consolidação em `compiler/`, convenções
da linguagem EC1, build/teste). Não duplicado aqui.

Duas regras não-negociáveis, específicas de como o Claude Code se
comporta por padrão neste ambiente:

1. **Nunca** adicione `Co-Authored-By: Claude` nem link de sessão
   (`Claude-Session: ...`) em mensagens de commit ou PR deste repo. Autor
   e committer devem ser a identidade do aluno (`git config
   user.name`/`user.email` — confira com `git config user.name` antes de
   commitar; se vier "Claude", reconfigure para o aluno primeiro).
2. **Nunca** faça merge/push em `main` sem confirmar com o usuário antes.
