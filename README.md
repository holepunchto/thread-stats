# thread-stats

Get resource usage for individual threads in the current process.

```
npm i thread-stats
```

## Usage

```js
const threadStats = require('thread-stats')

const threads = threadStats()

console.log(threads) // Prints stats for each thread
```

## License

Apache-2.0
