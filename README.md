# Welcome to RickMortyMiddleware
> Essa aplicaçao é um middleware que consome a API [rickandmortyapi](https://rickandmortyapi.com/documentation/#get-multiple-characters) 
> A aplicaçao possui uma camada de cache local que armazena personagens, locais e episodios em modelos locais evitando overload excessivo na API consumida.
> 
### Endpoints:  
`GET /character/all`  retorna todos os personsagens em um unico json  
`GET /character/<id>` retorna um personagem especifico pelo id  
`GET /character/<id>,<id>` retorna varios personagens especificados  
`GET /character/<?query>` retorna personagens que cumprem o filtro especificado

### Stack:
| Tecnologia                          | Função no projeto                                 |
| ----------------------------------- | ------------------------------------------------- |
| **C++20**                           | Linguagem principal do desafio                    |
| **Boost/Asio**                      | Networking (HTTP/HTTPS client + server)           |
| **Boost/Beast**                     | Engine HTTP e abstração de streams                |
| **Boost/JSON**                      | Parse e serialização de JSON                      |
| **GTest**                           | Testes unitários automatizados                    |
| **CMake**                           | Build system e automação de testes                |
| **Conan**                           | Gerenciador de dependências/pacotes               |
| **CMakeLists.txt**                  | Orquestra compilação e execução do test suite     |

### Estrutura do Projeto:
```
📁 RickMortyMiddle
├── 📁 include  
│   ├── api.hpp            Declara API do middleware + cache
│   ├── http_client.hpp    Interface do cliente HTTPS externo
│   ├── session.hpp        Sessão/Roteamento HTTP
│   ├── models.hpp         Models do domínio (Character, Episode e Location)
│   └── utils.hpp          Funções auxiliares
│  
├── 📁 src  
│   ├── api.cpp            Implementa consumo API externa + cache
│   ├── http_client.cpp    Implementa HTTPS (transporte)
│   ├── session.cpp        Controller HTTP (request, JSON response)
│   └── utils.cpp          Funções auxiliares
│  
├── 📁 tests  
│   ├── test_main.cpp      Inicializa GTest + testes unitários
│   └── test_endpoint.cpp  Testes de integração dos endpoints
│  
├── CMakeLists.txt         Orquestrador do build
├── conanfile.txt          Manifesto de dependências
└── CMakePresets.json      Configurações do CMake
```

---

# Install

### Package Manager

Instalação do Conan (caso não esteja disponível)
```
pip3 install --upgrade conan
conan profile detect
```

Verifique a instalação:
```
conan --version
```

### Build

1. Instalar dependências com Conan:
```
conan install . --output-folder=build --build=missing -s build_type=Release
```

2. Configurar o CMake usando o toolchain do Conan:
```
cmake -S . -B build/Release \
  -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake \
  -DCMAKE_BUILD_TYPE=Release
```

3. Compilar:
```
cmake --build build/Release
```

4. Ao final, o binário app será gerado em:
```
./build/Release/app
```

5. Executar o Middleware
```
./build/Release/app
```

---




