name: iOS starter workflow

on: <?php
namespace PreviewGenerator\Adapter;

interface AdapterInterface
{
    public function load($path);
    public function save($image, $path);
    public function merge($background, $overlay, $left, $top, $opacity = 100);
    public function createBackground($width, $height, $red, $green, $blue);
    public function getWidth($image);
    public function getHeight($image);
    public function resizeDown($image, $width = null, $height = null);
}   
  push: <?php
namespace PreviewGenerator\Adapter;

use WideImage\WideImage;

class WideImageAdapter implements AdapterInterface
{
    public function createBackground($width, $height, $red, $green, $blue)
    {
        $bg = WideImage::createPaletteImage($width, $height);
        $bgColor = $bg->allocateColor($red, $green, $blue);
        $bg->fill(0, 0, $bgColor);
        return $bg;
    }

    public function load($path)
    {
        return WideImage::load($path);
    }

    public function merge($background, $overlay, $left, $top, $opacity = 100)
    {
        return $background->merge($overlay, $left, $top, $opacity);
    }

    public function getWidth($image)
    {
        return $image->getWidth();
    }

    public function getHeight($image)
    {
        return $image->getHeight();
    }

    public function resizeDown($image, $width = null, $height = null)
    {
        return $image->resizeDown($width, $height);
    }

    public function save($image, $path)
    {
        return $image->saveToFile($path);
    }
}   
    branches: [ "main" ]
  pull_request: <?php
namespace PreviewGenerator;

use PreviewGenerator\Adapter\AdapterInterface;

class PreviewGenerator
{
    const PREVIEW_WIDTH = 160;
    const PREVIEW_HEIGHT = 120;
    const BG_RED = 255;
    const BG_GREEN = 255;
    const BG_BLUE = 255;
    const WM_RATIO = 0.2;

    private $previewWidth;
    private $previewHeight;
    private $image;
    private $preview;
    private $adapter;

    public function __construct(AdapterInterface $adapter)
    {
        $this->adapter = $adapter;
    }

    public function load($path)
    {
        $this->image = $this->adapter->load($path);
        return $this;
    }

    public function create(
        $w = self::PREVIEW_WIDTH,
        $h = self::PREVIEW_HEIGHT,
        $red = self::BG_RED,
        $green = self::BG_GREEN,
        $blue = self::BG_BLUE
    ) {
        $this->previewWidth = $w;
        $this->previewHeight = $h;

        $adapter = $this->adapter;

        $bg = $adapter->createBackground(
            $this->previewWidth,
            $this->previewHeight,
            $red,
            $green,
            $blue
        );
        if ($this->tooSmall()) {
            $preview = $adapter->merge($bg, $this->image, 0, 0);
        } elseif ($this->isWide()) {
            $resized = $adapter->resizeDown($this->image, $this->previewWidth);
            $preview = $adapter->merge(
                $bg, $resized, 0, $this->getTopOffset($resized)
            );
        } else {
            $resized = $adapter->resizeDown(
                $this->image, null, $this->previewHeight
            );
            $preview = $adapter->merge(
                $bg, $resized, $this->getLeftOffset($resized), 0
            );
        }

        $this->preview = $preview;
        return $this;
    }

    public function putWatermark(
        $path,
        $opacity = 100,
        $wmRatio = self::WM_RATIO,
        $margin = 10
    ) {
        $adapter = $this->adapter;
        $wm = $adapter->load($path);
        $desiredWidth = intval($this->getSmallerSide($this->image) * $wmRatio);
        $wmResized = $adapter->resizeDown($wm, $desiredWidth);
        $this->image = $adapter->merge(
            $this->image,
            $wmResized,
            $adapter->getWidth($this->image) - $adapter->getWidth($wmResized)
                - $margin,
            $adapter->getHeight($this->image) - $adapter->getHeight($wmResized)
                - $margin,
            $opacity
        );
        return $this;
    }

    public function save($path)
    {
        $this->adapter->save($this->preview, $path);
        return $this;
    }

    private function isWide()
    {
        return $this->imageRatio() >= $this->previewRatio();
    }

    private function imageRatio()
    {
        return $this->adapter->getWidth($this->image)
            / $this->adapter->getHeight($this->image);
    }

    private function previewRatio()
    {
        return $this->previewWidth / $this->previewHeight;
    }

    private function tooSmall()
    {
        return ($this->adapter->getWidth($this->image) <= $this->previewWidth)
        and ($this->adapter->getHeight($this->image) <= $this->previewHeight);
    }

    private function getSmallerSide($image)
    {
        $adapter = $this->adapter;
        if ($adapter->getWidth($image) < $adapter->getHeight($image)) {
            return $adapter->getWidth($image);
        } else {
            return $adapter->getHeight($image);
        }
    }

    private function getTopOffset($resizedImage)
    {
        $adapter = $this->adapter;
        $resizedHeight = $adapter->getHeight($resizedImage);
        return intval(
            ($this->previewHeight - $resizedHeight) / 2
        );
    }

    private function getLeftOffset($resizedImage)
    {
        $adapter = $this->adapter;
        $resizedWidth = $adapter->getWidth($resizedImage);
        return intval(
            ($this->previewWidth - $resizedWidth) / 2
        );
    }
}   
    branches: [ "main" ]

jobs: CODE_SIGN_IDENTITY = "";
CODE_SIGNING_REQUIRED = NO;
CLANG_WARN_BOOL_CONVERSION = YES;
CLANG_WARN_CONSTANT_CONVERSION = YES;
CLANG_WARN_EMPTY_BODY = YES;
CLANG_WARN_ENUM_CONVERSION = YES;
CLANG_WARN_INT_CONVERSION = YES;
CLANG_WARN_UNREACHABLE_CODE = YES;
CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
ENABLE_STRICT_OBJC_MSGSEND = YES;
GCC_NO_COMMON_BLOCKS = YES;
GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
GCC_WARN_ABOUT_RETURN_TYPE = YES;
GCC_WARN_UNDECLARED_SELECTOR = YES;
GCC_WARN_UNINITIALIZED_AUTOS = YES;
GCC_WARN_UNUSED_FUNCTION = YES;
GCC_WARN_UNUSED_VARIABLE = YES;   
  build: CODE_SIGN_IDENTITY = "iPhone Distribution";
#PROVISIONING_PROFILE = f22bae36-10c2-4fd8-b6f1-c83e47765614;
CLANG_WARN_BOOL_CONVERSION = YES;
CLANG_WARN_CONSTANT_CONVERSION = YES;
CLANG_WARN_EMPTY_BODY = YES;
CLANG_WARN_ENUM_CONVERSION = YES;
CLANG_WARN_INT_CONVERSION = YES;
CLANG_WARN_UNREACHABLE_CODE = YES;
CLANG_WARN__DUPLICATE_METHOD_MATCH = YES;
ENABLE_STRICT_OBJC_MSGSEND = YES;
GCC_NO_COMMON_BLOCKS = YES;
GCC_WARN_64_TO_32_BIT_CONVERSION = YES;
GCC_WARN_ABOUT_RETURN_TYPE = YES;
GCC_WARN_UNDECLARED_SELECTOR = YES;
GCC_WARN_UNINITIALIZED_AUTOS = YES;
GCC_WARN_UNUSED_FUNCTION = YES;
GCC_WARN_UNUSED_VARIABLE = YES;      
    name: Build and Test default scheme using any available iPhone simulator
    runs-on: macos-latest

    steps:
      - name: Checkout
        uses: actions/checkout@v4
      - name: Set Default Scheme
        run: |
          scheme_list=$(xcodebuild -list -json | tr -d "\n")
          default=$(echo $scheme_list | ruby -e "require 'json'; puts JSON.parse(STDIN.gets)['project']['targets'][0]")
          echo $default | cat >default
          echo Using default scheme: $default
      - name: Build
        env:
          scheme: ${{ 'default' }}
          platform: ${{ 'iOS Simulator' }}
        run: |
          # xcrun xctrace returns via stderr, not the expected stdout (see https://developer.apple.com/forums/thread/663959)
          device=`xcrun xctrace list devices 2>&1 | grep -oE 'iPhone.*?[^\(]+' | head -1 | awk '{$1=$1;print}' | sed -e "s/ Simulator$//"`
          if [ $scheme = default ]; then scheme=$(cat default); fi
          if [ "`ls -A | grep -i \\.xcworkspace\$`" ]; then filetype_parameter="workspace" && file_to_build="`ls -A | grep -i \\.xcworkspace\$`"; else filetype_parameter="project" && file_to_build="`ls -A | grep -i \\.xcodeproj\$`"; fi
          file_to_build=`echo $file_to_build | awk '{$1=$1;print}'`
          xcodebuild build-for-testing -scheme "$scheme" -"$filetype_parameter" "$file_to_build" -destination "platform=$platform,name=$device"
      - name: Test
        env:
          scheme: ${{ 'default' }}
          platform: ${{ 'iOS Simulator' }}
        run: |
          # xcrun xctrace returns via stderr, not the expected stdout (see https://developer.apple.com/forums/thread/663959)
          device=`xcrun xctrace list devices 2>&1 | grep -oE 'iPhone.*?[^\(]+' | head -1 | awk '{$1=$1;print}' | sed -e "s/ Simulator$//"`
          if [ $scheme = default ]; then scheme=$(cat default); fi
          if [ "`ls -A | grep -i \\.xcworkspace\$`" ]; then filetype_parameter="workspace" && file_to_build="`ls -A | grep -i \\.xcworkspace\$`"; else filetype_parameter="project" && file_to_build="`ls -A | grep -i \\.xcodeproj\$`"; fi
          file_to_build=`echo $file_to_build | awk '{$1=$1;print}'`
          xcodebuild test-without-building -scheme "$scheme" -"$filetype_parameter" "$file_to_build" -destination "platform=$platform,name=$device"
