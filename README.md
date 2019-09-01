
# Simple example that multiply all values of a 3x3 block by 2

```
// This is a comment

void mul2(block<3,3> input, block<3,3> output) {
   for (x,y in input) {
      output(x,y) = 2*input(x,y)
   }
}

```

