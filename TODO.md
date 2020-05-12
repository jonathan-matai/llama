# TO DO List

## Top Priority
1. No longer force Server ticks to happen 60x per second (Problem: Server can't keep up when framerate is limited to 60, e.g. on a laptop in battery mode)
1. Design an Entity System
1. Redesign the Renderer to connect to the List of entites
1. Synchronize the Entity List with the Server and other Clients

## Event System
- Handle Network Packet Loss
- Compare Event Size (Member of llama::Event) with expected Size of llama::Event subclass -> throw Error if the don't match
- Add support for Data Packets with Dynamic Size

## Error Handling
- Throw and catch std::runtime_error objects throughout the entire engine

## Math Library
- Matrix-Vector and Vector-Matrix Muliplication

## Graphics Engine
- Compare performance of rendering front-to-back compared to back-to-fromt
- Compare performance of rendering in random order compared to grouping by pipeline, vertex buffers, ...