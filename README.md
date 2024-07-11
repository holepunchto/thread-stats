# thread-stats

Get resource usage for individual threads in the current process.

```
npm i thread-stats
```

## Usage

```js
const threadStats = require('thread-stats')

const threads = threadStats()

console.log(threads) // All thread stats
console.log(threads.self) // Current thread stats
```

## License

Apache-2.0
