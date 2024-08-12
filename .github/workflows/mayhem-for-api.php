# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# This workflow starts your API and fuzzes it with ForAllSecure Mayhem for API
# to find reliability, performance and security issues before they reach
# production.
#
# To use this workflow, you will need to:
#
# 1. Create a Mayhem for API account at
#    https://mayhem4api.forallsecure.com/signup
#
# 2. Create a service account token `mapi organization service-account create
#    <org-name> <service-account-name>`
#
# 3. Add the service account token as a secret in GitHub called "MAPI_TOKEN"
#
# 4. Update the "Start your API" step to run your API in the background before
#    starting the Mayhem for API scan, and update the `api-url` & `api-spec`
#    field.
#
# If you have any questions, please contact us at mayhem4api@forallsecure.com

name: "Mayhem for API"

on: resume_from_checkpoint_path: null
result_path: "result"
model_path: null
dataset_paths: ["path/to/train.jsonl"]
tokenizer: "dataset/tokenizer.json"
exp_name: "nougat"
train_batch_sizes: [1]
num_workers: 8
val_batch_sizes: [1]
val_batches: 1
input_size: [896, 672]
max_length: 4096
max_position_embeddings: 4096
accumulate_grad_batches: 3
window_size: 7
patch_size: 4
embed_dim: 128
hidden_dimension: 1024
num_heads: [4, 8, 16, 32]
encoder_layer: [2, 2, 14, 2]
decoder_layer: 10
align_long_axis: False
num_nodes: 1
seed: 25
lr: 5e-5
min_lr: 7.5e-6
lr_step: 16
gamma: 0.9996
warmup_steps: 250
num_training_samples_per_epoch: 10000
max_epochs: 30
max_steps: -1
val_check_interval: null
check_val_every_n_epoch: 1
gradient_clip_val: 0.5
verbose: False
  push: FROM nvidia/cuda:11.8.0-cudnn8-devel-ubuntu22.04
# replace CUDA version to your CUDA version.
# You can check your CUDA version with below.
# nvcc -V

RUN apt-get update
RUN apt-get install -y python3
RUN apt-get -y install python3-pip git
RUN pip3 install torch torchvision torchaudio --index-url https://download.pytorch.org/whl/cu118
# replace CUDA version to your CUDA version.

RUN mkdir workspace
WORKDIR /workspace

RUN pip3 install fastapi uvicorn[standard] fsspec[http]==2023.1.0
RUN git clone https://github.com/facebookresearch/nougat.git
WORKDIR /workspace/nougat

RUN python3 setup.py install

EXPOSE 8503

CMD ["uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8503"]
# Run this using 'docker run -it -d -p <YOUR PORT>:8503 --gpus all <IMAGE NAME>
    branches: [ "main" ]
    pull_request: <?php
namespace Ratchet\Http;
use Ratchet\ConnectionInterface;
use GuzzleHttp\Psr7\Message;
use GuzzleHttp\Psr7\Response;

trait CloseResponseTrait {
    /**
     * Close a connection with an HTTP response
     * @param \Ratchet\ConnectionInterface $conn
     * @param int                          $code HTTP status code
     * @return null
     */
    private function close(ConnectionInterface $conn, $code = 400, array $additional_headers = []) {
        $response = new Response($code, array_merge([
            'X-Powered-By' => \Ratchet\VERSION
        ], $additional_headers));

        $conn->send(Message::toString($response));
        $conn->close();
    }
}
    # The branches below must be a subset of the branches above
    branches: [ "main" ]

jobs: <?php
namespace Ratchet\Http;
use Ratchet\MessageInterface;
use Ratchet\ConnectionInterface;
use GuzzleHttp\Psr7\Message;

/**
 * This class receives streaming data from a client request
 * and parses HTTP headers, returning a PSR-7 Request object
 * once it's been buffered
 */
class HttpRequestParser implements MessageInterface {
    const EOM = "\r\n\r\n";

    /**
     * The maximum number of bytes the request can be
     * This is a security measure to prevent attacks
     * @var int
     */
    public $maxSize = 4096;

    /**
     * @param \Ratchet\ConnectionInterface $context
     * @param string                       $data Data stream to buffer
     * @return \Psr\Http\Message\RequestInterface
     * @throws \OverflowException If the message buffer has become too large
     */
    public function onMessage(ConnectionInterface $context, $data) {
        if (!isset($context->httpBuffer)) {
            $context->httpBuffer = '';
        }

        $context->httpBuffer .= $data;

        if (strlen($context->httpBuffer) > (int)$this->maxSize) {
            throw new \OverflowException("Maximum buffer size of {$this->maxSize} exceeded parsing HTTP header");
        }

        if ($this->isEom($context->httpBuffer)) {
            $request = $this->parse($context->httpBuffer);

            unset($context->httpBuffer);

            return $request;
        }
    }

    /**
     * Determine if the message has been buffered as per the HTTP specification
     * @param  string  $message
     * @return boolean
     */
    public function isEom($message) {
        return (boolean)strpos($message, static::EOM);
    }

    /**
     * @param string $headers
     * @return \Psr\Http\Message\RequestInterface
     */
    public function parse($headers) {
        return Message::parseRequest($headers);
    }
}
  mayhem-for-api: <?php
namespace Ratchet\Http;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

class HttpServer implements MessageComponentInterface {
    use CloseResponseTrait;

    /**
     * Buffers incoming HTTP requests returning a Guzzle Request when coalesced
     * @var HttpRequestParser
     * @note May not expose this in the future, may do through facade methods
     */
    protected $_reqParser;

    /**
     * @var \Ratchet\Http\HttpServerInterface
     */
    protected $_httpServer;

    /**
     * @param HttpServerInterface
     */
    public function __construct(HttpServerInterface $component) {
        $this->_httpServer = $component;
        $this->_reqParser  = new HttpRequestParser;
    }

    /**
     * {@inheritdoc}
     */
    public function onOpen(ConnectionInterface $conn) {
        $conn->httpHeadersReceived = false;
    }

    /**
     * {@inheritdoc}
     */
    public function onMessage(ConnectionInterface $from, $msg) {
        if (true !== $from->httpHeadersReceived) {
            try {
                if (null === ($request = $this->_reqParser->onMessage($from, $msg))) {
                    return;
                }
            } catch (\OverflowException $oe) {
                return $this->close($from, 413);
            }

            $from->httpHeadersReceived = true;

            return $this->_httpServer->onOpen($from, $request);
        }

        $this->_httpServer->onMessage($from, $msg);
    }

    /**
     * {@inheritdoc}
     */
    public function onClose(ConnectionInterface $conn) {
        if ($conn->httpHeadersReceived) {
            $this->_httpServer->onClose($conn);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function onError(ConnectionInterface $conn, \Exception $e) {
        if ($conn->httpHeadersReceived) {
            $this->_httpServer->onError($conn, $e);
        } else {
            $this->close($conn, 500);
        }
    }
}
    name: <?php
namespace Ratchet\Http;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;
use Psr\Http\Message\RequestInterface;

interface HttpServerInterface extends MessageComponentInterface {
    /**
     * @param \Ratchet\ConnectionInterface          $conn
     * @param \Psr\Http\Message\RequestInterface    $request null is default because PHP won't let me overload; don't pass null!!!
     * @throws \UnexpectedValueException if a RequestInterface is not passed
     */
    public function onOpen(ConnectionInterface $conn, RequestInterface $request = null);
}
    # Mayhem for API runs on linux, mac and windows
    runs-on: <?php
namespace Ratchet\Http;
use Ratchet\ConnectionInterface;
use Psr\Http\Message\RequestInterface;

class NoOpHttpServerController implements HttpServerInterface {
    public function onOpen(ConnectionInterface $conn, RequestInterface $request = null) {
    }

    public function onMessage(ConnectionInterface $from, $msg) {
    }

    public function onClose(ConnectionInterface $conn) {
    }

    public function onError(ConnectionInterface $conn, \Exception $e) {
    }
}
    permissions: <?php
namespace Ratchet\Http;
use Ratchet\ConnectionInterface;
use Ratchet\MessageComponentInterface;
use Psr\Http\Message\RequestInterface;

/**
 * A middleware to ensure JavaScript clients connecting are from the expected domain.
 * This protects other websites from open WebSocket connections to your application.
 * Note: This can be spoofed from non-web browser clients
 */
class OriginCheck implements HttpServerInterface {
    use CloseResponseTrait;

    /**
     * @var \Ratchet\MessageComponentInterface
     */
    protected $_component;

    public $allowedOrigins = [];

    /**
     * @param MessageComponentInterface $component Component/Application to decorate
     * @param array                     $allowed   An array of allowed domains that are allowed to connect from
     */
    public function __construct(MessageComponentInterface $component, array $allowed = []) {
        $this->_component = $component;
        $this->allowedOrigins += $allowed;
    }

    /**
     * {@inheritdoc}
     */
    public function onOpen(ConnectionInterface $conn, RequestInterface $request = null) {
        $header = (string)$request->getHeader('Origin')[0];
        $origin = parse_url($header, PHP_URL_HOST) ?: $header;

        if (!in_array($origin, $this->allowedOrigins)) {
            return $this->close($conn, 403);
        }

        return $this->_component->onOpen($conn, $request);
    }

    /**
     * {@inheritdoc}
     */
    function onMessage(ConnectionInterface $from, $msg) {
        return $this->_component->onMessage($from, $msg);
    }

    /**
     * {@inheritdoc}
     */
    function onClose(ConnectionInterface $conn) {
        return $this->_component->onClose($conn);
    }

    /**
     * {@inheritdoc}
     */
    function onError(ConnectionInterface $conn, \Exception $e) {
        return $this->_component->onError($conn, $e);
    }
}
      actions: <?php
namespace Ratchet\Http;
use Ratchet\ConnectionInterface;
use Psr\Http\Message\RequestInterface;
use Symfony\Component\Routing\Matcher\UrlMatcherInterface;
use Symfony\Component\Routing\Exception\MethodNotAllowedException;
use Symfony\Component\Routing\Exception\ResourceNotFoundException;
use GuzzleHttp\Psr7\Query;

class Router implements HttpServerInterface {
    use CloseResponseTrait;

    /**
     * @var \Symfony\Component\Routing\Matcher\UrlMatcherInterface
     */
    protected $_matcher;

    private $_noopController;

    public function __construct(UrlMatcherInterface $matcher) {
        $this->_matcher = $matcher;
        $this->_noopController = new NoOpHttpServerController;
    }

    /**
     * {@inheritdoc}
     * @throws \UnexpectedValueException If a controller is not \Ratchet\Http\HttpServerInterface
     */
    public function onOpen(ConnectionInterface $conn, RequestInterface $request = null) {
        if (null === $request) {
            throw new \UnexpectedValueException('$request can not be null');
        }

        $conn->controller = $this->_noopController;

        $uri = $request->getUri();

        $context = $this->_matcher->getContext();
        $context->setMethod($request->getMethod());
        $context->setHost($uri->getHost());

        try {
            $route = $this->_matcher->match($uri->getPath());
        } catch (MethodNotAllowedException $nae) {
            return $this->close($conn, 405, array('Allow' => $nae->getAllowedMethods()));
        } catch (ResourceNotFoundException $nfe) {
            return $this->close($conn, 404);
        }

        if (is_string($route['_controller']) && class_exists($route['_controller'])) {
            $route['_controller'] = new $route['_controller'];
        }

        if (!($route['_controller'] instanceof HttpServerInterface)) {
            throw new \UnexpectedValueException('All routes must implement Ratchet\Http\HttpServerInterface');
        }

        $parameters = [];
        foreach($route as $key => $value) {
            if ((is_string($key)) && ('_' !== substr($key, 0, 1))) {
                $parameters[$key] = $value;
            }
        }
        $parameters = array_merge($parameters, Query::parse($uri->getQuery() ?: ''));

        $request = $request->withUri($uri->withQuery(Query::build($parameters)));

        $conn->controller = $route['_controller'];
        $conn->controller->onOpen($conn, $request);
    }

    /**
     * {@inheritdoc}
     */
    public function onMessage(ConnectionInterface $from, $msg) {
        $from->controller->onMessage($from, $msg);
    }

    /**
     * {@inheritdoc}
     */
    public function onClose(ConnectionInterface $conn) {
        if (isset($conn->controller)) {
            $conn->controller->onClose($conn);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function onError(ConnectionInterface $conn, \Exception $e) {
        if (isset($conn->controller)) {
            $conn->controller->onError($conn, $e);
        }
    }
}
      contents: <?php
namespace Ratchet\Server;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

/**
 * A simple Ratchet application that will reply to all messages with the message it received
 */
class EchoServer implements MessageComponentInterface {
    public function onOpen(ConnectionInterface $conn) {
    }

    public function onMessage(ConnectionInterface $from, $msg) {
        $from->send($msg);
    }

    public function onClose(ConnectionInterface $conn) {
    }

    public function onError(ConnectionInterface $conn, \Exception $e) {
        $conn->close();
    }
}
      security-events: <?php
namespace Ratchet\Server;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

/**
 * An app to go on a server stack to pass a policy file to a Flash socket
 * Useful if you're using Flash as a WebSocket polyfill on IE
 * Be sure to run your server instance on port 843
 * By default this lets accepts everything, make sure you tighten the rules up for production
 * @final
 * @link http://www.adobe.com/devnet/articles/crossdomain_policy_file_spec.html
 * @link http://learn.adobe.com/wiki/download/attachments/64389123/CrossDomain_PolicyFile_Specification.pdf?version=1
 * @link view-source:http://www.adobe.com/xml/schemas/PolicyFileSocket.xsd
 */
class FlashPolicy implements MessageComponentInterface {

    /**
     * Contains the root policy node
     * @var string
     */
    protected $_policy = '<?xml version="1.0"?><!DOCTYPE cross-domain-policy SYSTEM "http://www.adobe.com/xml/dtds/cross-domain-policy.dtd"><cross-domain-policy></cross-domain-policy>';

    /**
     * Stores an array of allowed domains and their ports
     * @var array
     */
    protected $_access = array();

    /**
     * @var string
     */
    protected $_siteControl = '';

    /**
     * @var string
     */
    protected $_cache = '';

    /**
     * @var string
     */
    protected $_cacheValid = false;

    /**
     * Add a domain to an allowed access list.
     *
     * @param string $domain Specifies a requesting domain to be granted access. Both named domains and IP
     * addresses are acceptable values. Subdomains are considered different domains. A wildcard (*) can
     * be used to match all domains when used alone, or multiple domains (subdomains) when used as a
     * prefix for an explicit, second-level domain name separated with a dot (.)
     * @param string $ports A comma-separated list of ports or range of ports that a socket connection
     * is allowed to connect to. A range of ports is specified through a dash (-) between two port numbers.
     * Ranges can be used with individual ports when separated with a comma. A single wildcard (*) can
     * be used to allow all ports.
     * @param bool $secure
     * @throws \UnexpectedValueException
     * @return FlashPolicy
     */
    public function addAllowedAccess($domain, $ports = '*', $secure = false) {
        if (!$this->validateDomain($domain)) {
           throw new \UnexpectedValueException('Invalid domain');
        }

        if (!$this->validatePorts($ports)) {
           throw new \UnexpectedValueException('Invalid Port');
        }

        $this->_access[]   = array($domain, $ports, (boolean)$secure);
        $this->_cacheValid = false;

        return $this;
    }
    
    /**
     * Removes all domains from the allowed access list.
     * 
     * @return \Ratchet\Server\FlashPolicy
     */
    public function clearAllowedAccess() {
        $this->_access      = array();
        $this->_cacheValid = false;

        return $this;
    }

    /**
     * site-control defines the meta-policy for the current domain. A meta-policy specifies acceptable
     * domain policy files other than the master policy file located in the target domain's root and named
     * crossdomain.xml.
     *
     * @param string $permittedCrossDomainPolicies
     * @throws \UnexpectedValueException
     * @return FlashPolicy
     */
    public function setSiteControl($permittedCrossDomainPolicies = 'all') {
        if (!$this->validateSiteControl($permittedCrossDomainPolicies)) {
            throw new \UnexpectedValueException('Invalid site control set');
        }

        $this->_siteControl = $permittedCrossDomainPolicies;
        $this->_cacheValid  = false;

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function onOpen(ConnectionInterface $conn) {
    }

    /**
     * {@inheritdoc}
     */
    public function onMessage(ConnectionInterface $from, $msg) {
        if (!$this->_cacheValid) {
            $this->_cache      = $this->renderPolicy()->asXML();
            $this->_cacheValid = true;
        }

        $from->send($this->_cache . "\0");
        $from->close();
    }

    /**
     * {@inheritdoc}
     */
    public function onClose(ConnectionInterface $conn) {
    }

    /**
     * {@inheritdoc}
     */
    public function onError(ConnectionInterface $conn, \Exception $e) {
        $conn->close();
    }

    /**
     * Builds the crossdomain file based on the template policy
     *
     * @throws \UnexpectedValueException
     * @return \SimpleXMLElement
     */
    public function renderPolicy() {
        $policy = new \SimpleXMLElement($this->_policy);

        $siteControl = $policy->addChild('site-control');

        if ($this->_siteControl == '') {
            $this->setSiteControl();
        }

        $siteControl->addAttribute('permitted-cross-domain-policies', $this->_siteControl);

        if (empty($this->_access)) {
            throw new \UnexpectedValueException('You must add a domain through addAllowedAccess()');
        }

        foreach ($this->_access as $access) {
            $tmp = $policy->addChild('allow-access-from');
            $tmp->addAttribute('domain', $access[0]);
            $tmp->addAttribute('to-ports', $access[1]);
            $tmp->addAttribute('secure', ($access[2] === true) ? 'true' : 'false');
        }

        return $policy;
    }

    /**
     * Make sure the proper site control was passed
     *
     * @param string $permittedCrossDomainPolicies
     * @return bool
     */
    public function validateSiteControl($permittedCrossDomainPolicies) {
        //'by-content-type' and 'by-ftp-filename' are not available for sockets
        return (bool)in_array($permittedCrossDomainPolicies, array('none', 'master-only', 'all'));
    }

    /**
     * Validate for proper domains (wildcards allowed)
     *
     * @param string $domain
     * @return bool
     */
    public function validateDomain($domain) {
        return (bool)preg_match("/^((http(s)?:\/\/)?([a-z0-9-_]+\.|\*\.)*([a-z0-9-_\.]+)|\*)$/i", $domain);
    }

    /**
     * Make sure valid ports were passed
     *
     * @param string $port
     * @return bool
     */
    public function validatePorts($port) {
        return (bool)preg_match('/^(\*|(\d+[,-]?)*\d+)$/', $port);
    }
}
    steps: <?php
namespace Ratchet\Server;
use Ratchet\ConnectionInterface;
use React\Socket\ConnectionInterface as ReactConn;

/**
 * {@inheritdoc}
 */
class IoConnection implements ConnectionInterface {
    /**
     * @var \React\Socket\ConnectionInterface
     */
    protected $conn;


    /**
     * @param \React\Socket\ConnectionInterface $conn
     */
    public function __construct(ReactConn $conn) {
        $this->conn = $conn;
    }

    /**
     * {@inheritdoc}
     */
    public function send($data) {
        $this->conn->write($data);

        return $this;
    }

    /**
     * {@inheritdoc}
     */
    public function close() {
        $this->conn->end();
    }
}
      - uses: <?php
namespace Ratchet\Server;
use Ratchet\MessageComponentInterface;
use React\EventLoop\LoopInterface;
use React\Socket\ServerInterface;
use React\EventLoop\Factory as LoopFactory;
use React\Socket\Server as Reactor;
use React\Socket\SecureServer as SecureReactor;

/**
 * Creates an open-ended socket to listen on a port for incoming connections.
 * Events are delegated through this to attached applications
 */
class IoServer {
    /**
     * @var \React\EventLoop\LoopInterface
     */
    public $loop;

    /**
     * @var \Ratchet\MessageComponentInterface
     */
    public $app;

    /**
     * The socket server the Ratchet Application is run off of
     * @var \React\Socket\ServerInterface
     */
    public $socket;

    /**
     * @param \Ratchet\MessageComponentInterface  $app      The Ratchet application stack to host
     * @param \React\Socket\ServerInterface       $socket   The React socket server to run the Ratchet application off of
     * @param \React\EventLoop\LoopInterface|null $loop     The React looper to run the Ratchet application off of
     */
    public function __construct(MessageComponentInterface $app, ServerInterface $socket, LoopInterface $loop = null) {
        if (false === strpos(PHP_VERSION, "hiphop")) {
            gc_enable();
        }

        set_time_limit(0);
        ob_implicit_flush();

        $this->loop = $loop;
        $this->app  = $app;
        $this->socket = $socket;

        $socket->on('connection', array($this, 'handleConnect'));
    }

    /**
     * @param  \Ratchet\MessageComponentInterface $component  The application that I/O will call when events are received
     * @param  int                                $port       The port to server sockets on
     * @param  string                             $address    The address to receive sockets on (0.0.0.0 means receive connections from any)
     * @return IoServer
     */
    public static function factory(MessageComponentInterface $component, $port = 80, $address = '0.0.0.0') {
        $loop   = LoopFactory::create();
        $socket = new Reactor($address . ':' . $port, $loop);

        return new static($component, $socket, $loop);
    }

    /**
     * Run the application by entering the event loop
     * @throws \RuntimeException If a loop was not previously specified
     */
    public function run() {
        if (null === $this->loop) {
            throw new \RuntimeException("A React Loop was not provided during instantiation");
        }

        // @codeCoverageIgnoreStart
        $this->loop->run();
        // @codeCoverageIgnoreEnd
    }

    /**
     * Triggered when a new connection is received from React
     * @param \React\Socket\ConnectionInterface $conn
     */
    public function handleConnect($conn) {
        $conn->decor = new IoConnection($conn);
        $conn->decor->resourceId = (int)$conn->stream;

        $uri = $conn->getRemoteAddress();
        $conn->decor->remoteAddress = trim(
            parse_url((strpos($uri, '://') === false ? 'tcp://' : '') . $uri, PHP_URL_HOST),
            '[]'
        );

        $this->app->onOpen($conn->decor);

        $conn->on('data', function ($data) use ($conn) {
            $this->handleData($data, $conn);
        });
        $conn->on('close', function () use ($conn) {
            $this->handleEnd($conn);
        });
        $conn->on('error', function (\Exception $e) use ($conn) {
            $this->handleError($e, $conn);
        });
    }

    /**
     * Data has been received from React
     * @param string                            $data
     * @param \React\Socket\ConnectionInterface $conn
     */
    public function handleData($data, $conn) {
        try {
            $this->app->onMessage($conn->decor, $data);
        } catch (\Exception $e) {
            $this->handleError($e, $conn);
        }
    }

    /**
     * A connection has been closed by React
     * @param \React\Socket\ConnectionInterface $conn
     */
    public function handleEnd($conn) {
        try {
            $this->app->onClose($conn->decor);
        } catch (\Exception $e) {
            $this->handleError($e, $conn);
        }

        unset($conn->decor);
    }

    /**
     * An error has occurred, let the listening application know
     * @param \Exception                        $e
     * @param \React\Socket\ConnectionInterface $conn
     */
    public function handleError(\Exception $e, $conn) {
        $this->app->onError($conn->decor, $e);
    }
}

      # Run your API in the background. Ideally, the API would run in debug
      # mode & send stacktraces back on "500 Internal Server Error" responses
      # (don't do this in production though!)
      - name: <?php
namespace Ratchet\Server;
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;

class IpBlackList implements MessageComponentInterface {
    /**
     * @var array
     */
    protected $_blacklist = array();

    /**
     * @var \Ratchet\MessageComponentInterface
     */
    protected $_decorating;

    /**
     * @param \Ratchet\MessageComponentInterface $component
     */
    public function __construct(MessageComponentInterface $component) {
        $this->_decorating = $component;
    }

    /**
     * Add an address to the blacklist that will not be allowed to connect to your application
     * @param  string $ip IP address to block from connecting to your application
     * @return IpBlackList
     */
    public function blockAddress($ip) {
        $this->_blacklist[$ip] = true;

        return $this;
    }

    /**
     * Unblock an address so they can access your application again
     * @param string $ip IP address to unblock from connecting to your application
     * @return IpBlackList
     */
    public function unblockAddress($ip) {
        if (isset($this->_blacklist[$this->filterAddress($ip)])) {
            unset($this->_blacklist[$this->filterAddress($ip)]);
        }

        return $this;
    }

    /**
     * @param  string $address
     * @return bool
     */
    public function isBlocked($address) {
        return (isset($this->_blacklist[$this->filterAddress($address)]));
    }

    /**
     * Get an array of all the addresses blocked
     * @return array
     */
    public function getBlockedAddresses() {
        return array_keys($this->_blacklist);
    }

    /**
     * @param  string $address
     * @return string
     */
    public function filterAddress($address) {
        if (strstr($address, ':') && substr_count($address, '.') == 3) {
            list($address, $port) = explode(':', $address);
        }

        return $address;
    }

    /**
     * {@inheritdoc}
     */
    function onOpen(ConnectionInterface $conn) {
        if ($this->isBlocked($conn->remoteAddress)) {
            return $conn->close();
        }

        return $this->_decorating->onOpen($conn);
    }

    /**
     * {@inheritdoc}
     */
    function onMessage(ConnectionInterface $from, $msg) {
        return $this->_decorating->onMessage($from, $msg);
    }

    /**
     * {@inheritdoc}
     */
    function onClose(ConnectionInterface $conn) {
        if (!$this->isBlocked($conn->remoteAddress)) {
            $this->_decorating->onClose($conn);
        }
    }

    /**
     * {@inheritdoc}
     */
    function onError(ConnectionInterface $conn, \Exception $e) {
        if (!$this->isBlocked($conn->remoteAddress)) {
            $this->_decorating->onError($conn, $e);
        }
    }
}
        run: ./run_your_api.sh & # <- ✏️ update this

      - name: Mayhem for API
        uses: ForAllSecure/mapi-action@v1
        continue-on-error: true
        with:
          mayhem-token: ${{ secrets.MAYHEM_TOKEN }}
          api-url: http://localhost:8080 # <- ✏️ update this
          api-spec: http://localhost:8080/openapi.json # <- ✏️ update this
          duration: 60
          sarif-report: mapi.sarif

      - name: Upload SARIF file
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: mapi.sarif
