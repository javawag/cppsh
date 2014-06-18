pkgname=cppsh
pkgver=r49.3068a42
pkgrel=1
pkgdesc="cppsh - C++11 as a scripting language, using CMake as a build system"
arch=('i686' 'x86_64' 'armv7h')
license=('custom')
url="https://github.com/javawag/cppsh"
depends=('glibc' "cmake" "boost-libs")
makedepends=("cmake")
checkdepends=()
provide=("cppsh")
options=('!libtool' '!strip')
source=(
  'git://github.com/javawag/cppsh.git'
  "${pkgname}.patch"
)
md5sums=('SKIP' "SKIP")

pkgver() {
  cd ${srcdir}/${pkgname}

  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
  cd ${srcdir}
  patch -p0 -i "${srcdir}/${pkgname}.patch"
}

build() {
  rm -rf ${srcdir}/${pkgname}-build
  mkdir -p ${srcdir}/${pkgname}-build
  cd ${srcdir}/${pkgname}-build
  cmake \
    -DCMAKE_CXX_FLAGS="${CXXFLAGS}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr \
    ${srcdir}/${pkgname}
}

package() {
  cd ${srcdir}/${pkgname}-build
  make DESTDIR=${pkgdir} install
}
