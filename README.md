# Polysync

Polysync is a command line tool for developing Polytoria games.

Inspired by tools like Rojo, Polysync allows developers to organize their projects locally,
process source files, syncronize scripts with Polytoria, and download Polytoria luau definition files.

> Polysync is still in heavy early development.

## Building

```bash
git clone https://github.com/elijahkomorebi/polysync
cd polysync
cmake -B build
cmake --build build
```

## Configuration

Polysync projects are configured using a `polysync.json` file.

Example:

```json
{
    "name": "Project",
    "tree": {
        "source": {
            "client": "Hidden",
            "server": "ScriptService",
            "shared": "Hidden"
        }
    }
}
```
