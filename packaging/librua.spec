
Name:       librua
Summary:    Recently used application
Version:    0.1.5
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
%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif
cmake . -DCMAKE_INSTALL_PREFIX=%{_prefix}


make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
%make_install
mkdir -p %{buildroot}/usr/share/license
install LICENSE %{buildroot}/usr/share/license/%{name}

%post
/sbin/ldconfig
mkdir -p /opt/dbspace/
sqlite3 /opt/dbspace/.rua.db < /opt/share/rua_db.sql
rm -rf /opt/share/rua_db.sql
chown 0:5000 /opt/dbspace/.rua.db
chown 0:5000 /opt/dbspace/.rua.db-journal
chmod 660 /opt/dbspace/.rua.db
chmod 660 /opt/dbspace/.rua.db-journal
chsmack -a rua::db /opt/dbspace/.rua.db
chsmack -a rua::db /opt/dbspace/.rua.db-journal

%postun -p /sbin/ldconfig



%files
%manifest librua.manifest
%defattr(-,root,root,-)
%config(missingok) /opt/share/rua_db.sql
/usr/lib/librua.so.*
/usr/share/license/%{name}



%files devel
%defattr(-,root,root,-)
/usr/include/rua/*.h
/usr/lib/librua.so
/usr/lib/pkgconfig/rua.pc

