class Foo {
  constructor (x=33) {
    this.x = x ? x : 99
    if (this.x) {
      console.info('covered')
    } else {
      console.info('uncovered')
    }
    this.methodC()
  }
  methodA () {
    console.info('covered')
  }
  methodB () {
    console.info('uncovered')
  }
  methodC () {
    console.info('covered')
  }
  methodD () {
    console.info('uncovered')
  }
}

const a = new Foo(0)
const b = new Foo(33)
a.methodA()
