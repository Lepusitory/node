'use strict'

const { InvalidArgumentError, RequestAbortedError, SocketError } = require('../core/errors')
const { AsyncResource } = require('async_hooks')
const util = require('../core/util')
const { addSignal, removeSignal } = require('./abort-signal')

class ConnectHandler extends AsyncResource {
  constructor (opts, callback) {
    if (!opts || typeof opts !== 'object') {
      throw new InvalidArgumentError('invalid opts')
    }

    if (typeof callback !== 'function') {
      throw new InvalidArgumentError('invalid callback')
    }

    const { signal, opaque, responseHeaders, httpTunnel } = opts

    if (signal && typeof signal.on !== 'function' && typeof signal.addEventListener !== 'function') {
      throw new InvalidArgumentError('signal must be an EventEmitter or EventTarget')
    }

    super('UNDICI_CONNECT')

    this.opaque = opaque || null
    this.responseHeaders = responseHeaders || null
    this.callback = callback
    this.abort = null
    this.httpTunnel = httpTunnel

    addSignal(this, signal)
  }

  onConnect (abort, context) {
    if (!this.callback) {
      throw new RequestAbortedError()
    }

    this.abort = abort
    this.context = context
  }

  onHeaders (statusCode) {
    // when httpTunnel headers are allowed
    if (this.httpTunnel) {
      const { callback, opaque } = this
      if (statusCode !== 200) {
        if (callback) {
          this.callback = null
          const err = new RequestAbortedError('Proxy response !== 200 when HTTP Tunneling')
          queueMicrotask(() => {
            this.runInAsyncScope(callback, null, err, { opaque })
          })
        }
        return 1
      }
    } else {
      throw new SocketError('bad connect', null)
    }
  }

  onUpgrade (statusCode, rawHeaders, socket) {
    const { callback, opaque, context } = this

    removeSignal(this)

    this.callback = null
    const headers = this.responseHeaders === 'raw' ? util.parseRawHeaders(rawHeaders) : util.parseHeaders(rawHeaders)
    this.runInAsyncScope(callback, null, null, {
      statusCode,
      headers,
      socket,
      opaque,
      context
    })
  }

  onError (err) {
    const { callback, opaque } = this

    removeSignal(this)

    if (callback) {
      this.callback = null
      queueMicrotask(() => {
        this.runInAsyncScope(callback, null, err, { opaque })
      })
    }
  }
}

function connect (opts, callback) {
  if (callback === undefined) {
    return new Promise((resolve, reject) => {
      connect.call(this, opts, (err, data) => {
        return err ? reject(err) : resolve(data)
      })
    })
  }

  try {
    const connectHandler = new ConnectHandler(opts, callback)
    this.dispatch({ ...opts, method: 'CONNECT' }, connectHandler)
  } catch (err) {
    if (typeof callback !== 'function') {
      throw err
    }
    const opaque = opts && opts.opaque
    queueMicrotask(() => callback(err, { opaque }))
  }
}

module.exports = connect
