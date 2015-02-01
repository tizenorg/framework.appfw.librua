
Name:       librua
Summary:    Recently used application
Version:    0.1.8
Release:    1
Group:      System/Libraries
License:    Apache License, Version 2.0
Source0:    librua-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(post): /usr/bin/sqlite3
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(dlog)


%description
Recently used application library



%package devel
Summary:    Recently used application (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Recently used application library (devel)


%prep
%setup -q -n %{name}-%{version}


%build
%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/usr/share/license
install LICENSE %{buildroot}/usr/share/license/%{name}

mkdir -p %{buildroot}/opt/dbspace
sqlite3 %{buildroot}/opt/dbspace/.rua.db < %{buildroot}/opt/share/rua_db.sql
rm -rf %{buildroot}/opt/share/rua_db.sql

%post
/sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%manifest librua.manifest
%defattr(-,root,root,-)
/usr/lib/librua.so.*
/usr/share/license/%{name}
%attr(660,root,app) /opt/dbspace/.rua.db
%attr(660,root,app) /opt/dbspace/.rua.db-journal

%files devel
%defattr(-,root,root,-)
/usr/include/rua/*.h
/usr/lib/librua.so
/usr/lib/pkgconfig/rua.pc

